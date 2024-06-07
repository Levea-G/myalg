import tkinter as tk
from tkinter import messagebox
import threading
import time
import socket
from PIL import ImageTk
import tools
import re
server=('240e:389:944:a000:d7ea:b906:ac13:4f05',1112)
try:
    from ctypes import windll
    windll.user32.SetProcessDPIAware(True)
except:pass
class form:
    def __init__(self,user:str,sock:socket.socket)->None:
        def handin()->None:
            symbol=self.form[0].get()
            assoc=tools.getassoc(symbol)
            if assoc[0]==[] or len(assoc)>1:
                self.formhint.config(text='股票代码不存在')
                return
            try:
                tarprice='%.2f'%float(self.form[1].get())
                amount=str(int(self.form[2].get())*100)
            except ValueError:
                self.formhint.config(text='价格或数量输入有误')
                return
            dir='1' if self.bs.get()=='买入' else '0'
            try:self.sock.sendto(('\x03add %s %s %s %s %s'%(self.user,symbol,tarprice,amount,dir)).encode(),server)
            except OSError:
                self.formhint.config(text='无网络，请重新打开页面')
                return
            try:buff,addr=self.sock.recvfrom(128)
            except (ConnectionResetError,socket.timeout):
                self.formhint.config(text='无法连接到服务器，请稍后再试')
                return
            except OSError:return
            if addr[:2]!=server:
                self.formhint.config(text='throttled')
                return
            buff=buff.decode().split(' ')
            if buff[0]=='\x03add':self.main.destroy()
            elif buff[0]=='\x03noown':self.formhint.config(text='持仓数量不足以出售')
            elif buff[0]=='\x03nocash':self.formhint.config(text='可用资金不足以购买')
            elif buff[0]=='\x03null':self.formhint.config(text='未找到用户，请重新打开本页面')
            elif buff[0]=='\x03freq':self.formhint.config(text='请求过于频繁')
        self.main=tk.Toplevel()
        self.main.geometry('480x450')
        self.user=user
        self.sock=sock
        tk.Label(self.main,font=('YaHei',24),text='提交委托').place(x=100,y=30,width=280,height=40)
        self.form:list[tk.Entry]=[]
        for i,name in enumerate(('股票代码','目标价格','委托数量(手)')):
            tk.Label(self.main,font=('YaHei',16),text=name,anchor='e').place(x=0,y=90+70*i,width=180,height=30)
            tentry=tk.Entry(self.main,font=('YaHei',16))
            tentry.place(x=200,y=90+70*i,width=240,height=30)
            self.form.append(tentry)
        tk.Label(self.main,font=('YaHei',16),text='买入/卖出',anchor='e').place(x=0,y=300,width=180,height=30)
        self.bs=tk.StringVar(self.main,'买入')
        tk.OptionMenu(self.main,self.bs,'买入','卖出').place(x=200,y=300,width=120,height=30)
        self.formhint=tk.Label(self.main,font=('YaHei',12),fg='#000505')
        self.formhint.place(x=0,y=350,width=480,height=30)
        tk.Button(self.main,font=('YaHei',12),text='确认提交',command=handin).place(x=100,y=400,width=280,height=30)
class client:
    def __init__(self)->None:
        def exitTk()->None:
            self.alive=False
            togglegrab()
            self.main.quit()
            self.main.destroy()
        def swcpage(init:bool=True)->None:
            page=self.page.get()
            self.frame[page].lift()
            if page in (0,3):
                self.table.lift()
            if not init:return
            if page in (0,3):
                self.tpage=1
                self.cell[self.currsort+1].config(fg='#FFFFFF')
                self.cell[1].config(fg='#8942C4')
                self.currsort=0
                self.asc=0
                self.node.set('hs_a')
                for i in range(self.select*18+1,self.select*18+18):
                    self.cell[i].config(bg='#000000')
                self.select=1
                for i in range(19,36):
                    self.cell[i].config(bg='#000099')
                for label in self.cell[18:]:label.config(text='')
                if page==0:self.maxpage=tools.getmaxpage('hs_a')
                elif page==3:
                    self.search.set('')
                    self.choice=getchoicelist()
                    self.maxpage=getmychoicepage()
            elif page==6:
                self.graph.delete('all')
                self.title.config(text='')
                self.titlep.config(text='')
                for lab in self.io5p+self.io5v+self.detail:
                    lab.config(text='')
                self.chshint.config(text='')
                self.axis.config(text='')
                self.infolist=[]
            elif page==5:
                self.user_in.delete('0','end')
                self.psw_in.delete('0','end')
                self.loghint.config(text='')
            elif page==2:
                self.usrhint.config(text='')
            elif page==4:
                self.hpage=1
                self.opage=1
                self.hmaxpage,self.omaxpage=gettradepage()
                self.hmaxpage=max(1,self.hmaxpage)
                self.omaxpage=max(1,self.omaxpage)
                for label in self.history[10:]+self.own[9:]:
                    label.config(text='')
            togglegrab()
        def swcnode()->None:
            self.tpage=1
            self.maxpage=tools.getmaxpage(self.node.get())
            togglegrab()
        def returnto()->None:
            page=self.page.get()
            self.page.set(self.fromp)
            swcpage(False if page==0 else True)
        def togglegrab()->None:
            self.lock.acquire()
            self.needgrab=True
            self.cond.notify()
            self.lock.release()
        def configlabel(val1:str,val2:str,labels:tuple[tk.Label])->None:
            if val1=='-' or val2=='-':tarc='#FFFFFF'
            else:
                v1=float(val1)
                v2=float(val2)
                if v1<v2:tarc='#00FF00'
                elif v2<v1:tarc='#FF0000'
                else:tarc='#FFFFFF'
            for lab in labels:lab.config(fg=tarc)
        def updatetable(data:list)->None:
            pageloc=(self.tpage-1)*25
            for i,infolist in enumerate(data,start=1):
                rowloc:int=i*18
                self.cell[rowloc].config(text=str(pageloc+i))
                configlabel(infolist[2],infolist[7],(self.cell[rowloc+i] for i in range(3,8)))
                configlabel(infolist[8],infolist[7],(self.cell[rowloc+9],))
                configlabel(infolist[9],infolist[7],(self.cell[rowloc+10],))
                configlabel(infolist[10],infolist[7],(self.cell[rowloc+11],))
                for j,val in enumerate(infolist,start=1):
                    self.cell[rowloc+j].config(text=val)
            for i in range(len(data)*18+18,468):
                self.cell[i].config(text='')
        def updatedetail(data:list)->None:
            if data==[]:return
            info,img=data
            # info=data
            configlabel(info[1],info[2],(self.titlep,self.detail[4],self.detail[5]))
            configlabel(info[3],info[2],(self.detail[1],))
            for val,label in zip(info[26:],self.io5p):
                configlabel(val,info[2],(label,))
            self.title.config(text=info[0])
            self.titlep.config(text=info[1])
            for val,label in zip(info[2:],self.detail+self.io5v[:-1]+self.io5p[:-1]):
                label.config(text=val)
            self.io5p[-1].config(text=info[2])
            self.img=ImageTk.PhotoImage(img,(1440,840))
            self.graph.create_image(720,420,image=self.img)
        def updateuserinfo(data:list)->None:
            if len(data)<2:return
            for entry,txt in zip(self.usrinfo,data[1:]):
                entry.config(state='normal')
                entry.delete('0','end')
                entry.insert('end',txt)
                entry.config(state='disabled')
            self.usrinfo[3].delete('0','end')
            self.usrinfo[4].delete('0','end')
            self.email=data[2]
        def updatehist_own(data:list)->None:
            if data==[]:return
            for i,slice in enumerate(data[0],start=1):
                rowloc=i*10
                self.history[rowloc].config(text=str(self.hpage*25-25+i))
                slice[2]='买入' if slice[2]=='1' else '卖出'
                slice[8]='未成交' if slice[8]=='0' else ('已成交' if slice[8]=='1' else '已撤销')
                for j,val in enumerate(slice,start=1):
                    self.history[rowloc+j].config(text=val)
            for i in range(len(data[0])*10+10,60):
                self.history[i].config(text='')
            for i,slice in enumerate(data[1],start=1):
                rowloc=i*9
                self.own[rowloc].config(text=str(self.opage*25-25+i))
                for j,val in enumerate(slice,start=1):
                    self.own[rowloc+j].config(text=val)
            for i in range(len(data[1])*9+9,54):
                self.own[i].config(text='')
        def getmktinfo()->list:
            return tools.getmktinfo(self.tpage,self.sort[self.currsort],self.asc,self.node.get())
        def getstkdetail()->list:
            try:data=tools.getstkinfo([self.symbol])[0]
            except IndexError:return []
            self.infolist=tools.getminline(self.symbol)
            return [data,tools.drawtoday(self.infolist,float(data[2]),float(data[4]),float(data[5]))]
        def getuserinfo()->list:
            if self.user=='':return []
            buffer=try_get('\x01get '+self.user,self.usrhint).split(' ')
            return buffer
        def getmychoice()->list:
            return tools.getstkinfo(self.choice,True)
        def gethist_own()->list:
            buff=try_get('\x03get %s %d %d'%(self.user,self.hpage-1,self.opage-1),self.tradehint,1024).split(' ')
            if buff[0]!='\x03got':return []
            return [[item.split(',') for item in buff[1].split(';')] if buff[1]!='' else [],tools.getstksimple([item.split(',') for item in buff[2].split(';')])]
        def timer()->None:
            while self.alive:
                time.sleep(5)
                if self.page.get() in (0,3,6):togglegrab()
        def grabber()->None:
            func:list=[
                getmktinfo,
                tools.voidcall,
                getuserinfo,
                getmychoice,
                gethist_own,
                tools.voidcall,
                getstkdetail,
            ]
            updatefunc:list=[
                updatetable,
                tools.voidcall,
                updateuserinfo,
                updatetable,
                updatehist_own,
                tools.voidcall,
                updatedetail,
            ]
            while self.alive:
                page=self.page.get()
                data=func[page]()
                if not self.alive:break
                if not self.needgrab:
                    self.tarpage=page
                    updatefunc[page](data)
                self.lock.acquire()
                while not self.needgrab and self.alive:self.cond.wait()
                self.needgrab=False
                self.lock.release()
        def cellclick(event:tk.Event)->None:
            if self.page.get() not in (0,3):return
            name:str=event.widget.winfo_name()
            if name[:4]!='cell':return
            index:int=int(name[4:])
            if index<18:
                if self.page.get()!=0:return
                if self.currsort==index-1:self.asc^=1
                else:
                    self.cell[self.currsort+1].config(fg='#FFFFFF')
                    self.cell[index].config(fg='#8942C4')
                    self.currsort=index-1
                    self.asc=0
                togglegrab()
            else:
                if self.select==index//18:return
                index-=index%18
                for i in range(self.select*18+1,self.select*18+18):
                    self.cell[i].config(bg='#000000')
                for i in range(index+1,index+18):
                    self.cell[i].config(bg='#000099')
                self.select=index//18
        def cellselect(event:tk.Event)->None:
            name:str=event.widget.winfo_name()
            if name[:4]!='cell':return
            index:int=int(name[4:])
            if index<18:return
            self.symbol=self.cell[index-index%18+1].cget('text')
            self.fromp=self.page.get()
            self.page.set(6)
            swcpage()
        def scroll(event:tk.Event)->None:
            if self.page.get() not in (0,3):return
            if event.delta<0:newpage=self.tpage+1
            else:newpage=self.tpage-1
            newpage=(newpage-1)%self.maxpage+1
            if newpage==self.tpage:return
            self.tpage=newpage
            if self.page.get()==3:self.choice=getchoicelist()
            togglegrab()
        def try_bind()->None:
            for info in socket.getaddrinfo(socket.gethostname(),None,socket.AF_INET6):
                if info[-1][0]!='::1' and info[-1][-1]==0:
                    try:self.sock.bind((info[-1][0],0))
                    except:continue
                    break
        def try_get(request:str,log:tk.Label,buffsize:int=128)->str:
            try:self.sock.sendto(request.encode(),server)
            except OSError:
                try_bind()
                try:self.sock.sendto(request.encode(),server)
                except OSError:
                    log.config(text='无网络连接')
                    return ''
            try:buffer,addr=self.sock.recvfrom(buffsize)
            except (ConnectionResetError,socket.timeout):
                log.config(text='无法连接到服务器，请稍后再试')
                return ''
            except OSError:return ''
            if addr[:2]!=server:
                log.config(text='throttled')
                return ''
            return buffer.decode()
        def asklog()->None:
            usrname=self.user_in.get()
            psw=self.psw_in.get()
            if len(usrname)>30 or len(psw)>20:
                self.loghint.config(text='用户不存在或密码错误')
                return
            buffer=try_get('\x00log '+usrname+' '+psw,self.loghint).split(' ')
            if buffer[0]=='\x00permit':
                self.topbtn[-1].place_forget()
                for i in range(2,5):self.topbtn[i].place(x=i*200,y=0,width=200,height=30)
                self.user=buffer[1]
                self.page.set(2)
                swcpage()
            elif buffer[0]=='\x00forbid':self.loghint.config(text='用户不存在或密码错误')
        def askreg()->None:
            usrname=self.user_in.get()
            psw=self.psw_in.get()
            if len(usrname)>30:
                self.loghint.config(text='邮箱长度不得超过30字符')
                return
            stat=tools.checkinfo(usrname,psw)
            if stat==1:
                self.loghint.config(text='邮箱格式不正确')
                return
            elif stat==2:
                self.loghint.config(text='密码含有非法字符')
                return
            buffer=try_get('\x00reg '+usrname+' '+psw,self.loghint).split(' ')
            if buffer[0]=='\x00permit':
                self.topbtn[-1].place_forget()
                for i in range(2,5):self.topbtn[i].place(x=i*200,y=0,width=200,height=30)
                self.user=buffer[1]
                self.page.set(2)
                swcpage()
            elif buffer[0]=='\x00repeat':self.loghint.config(text='邮箱已被使用，注册失败')
        def askmodify()->None:
            info=[entry.get() for entry in self.usrinfo]
            if len(info[1])>30:
                self.usrhint.config(text='邮箱长度不得超过30字符')
                return
            stat=tools.checkinfo(info[1],info[3])
            if stat==1:
                self.usrhint.config(text='邮箱格式不正确')
                return
            elif stat==2:
                self.usrhint.config(text='密码含有非法字符')
                return
            buffer=try_get('\x01set %s %s %s %s %s'%(self.user,info[1],info[2],info[3],info[4]),self.usrhint).split(' ')
            if len(buffer)<1:return
            if buffer[0]=='\x01set':
                self.usrhint.config(text='修改成功')
                togglegrab()
            elif buffer[0]=='\x01psw':
                self.usrhint.config(text='原密码错误')
            elif buffer[0]=='\x01repeat':
                self.usrhint.config(text='邮箱已被占用')
        def swcshow(entry:tk.Entry)->None:
            if entry.cget('show')=='':entry.config(show='*')
            else:entry.config(show='')
        def swcstat()->None:
            if self.chgstat.get()==1:
                self.usrinfo[1].config(state='normal')
            else:
                self.usrinfo[1].delete('0','end')
                self.usrinfo[1].insert('0',self.email)
                self.usrinfo[1].config(state='disabled')
        def logout()->None:
            self.user=self.email=''
            for i in range(2,5):self.topbtn[i].place_forget()
            self.topbtn[-1].place(x=400,y=0,width=200,height=30)
            self.page.set(5)
            swcpage()
        def chgchoice(symbol:str,dir:bool)->None:
            if self.user=='':
                self.chshint.config(text='未登录')
                return
            buff=try_get(('\x02add ' if dir else '\x02del ')+self.user+' '+symbol,self.chshint).split(' ')
            if buff[0]=='\x02add':self.chshint.config(text='添加成功')
            elif buff[0]=='\x02del':self.chshint.config(text='移除成功')
            elif buff[0]=='\x02has':self.chshint.config(text='已经在自选列表中')
            elif buff[0]=='\x02null':self.chshint.config(text='不在自选列表中')
        def getchoicelist()->list:
            if self.user=='':return ['']
            buff=try_get('\x02get %s %d'%(self.user,self.tpage),self.myhint,512).split(' ')
            if buff[0]=='\x02got':
                return buff[1].split(',')
            else:return ['']
        def getmychoicepage()->int:
            buff=try_get('\x02ct '+self.user,self.myhint).split(' ')
            if buff[0]=='\x02ct':return int(buff[1])//25+1
            elif buff[0]=='\x02null':return 1
        def showassoc(*args)->None:
            self.assoclist.delete('0','end')
            currword=self.search.get()
            if currword=='':return
            symbol_list=tools.getassoc(currword)
            for symbol in symbol_list:
                self.assoclist.insert('end',symbol)
            if len(symbol_list)==0:hideassoc()
            else:
                self.assoclist.place(x=100,y=80,width=300,height=150)
                self.assoclist.lift()
        def hideassoc(event:tk.Event)->None:
            if self.page.get()!=3:return
            name=event.widget.winfo_name()
            if name!='assoc' and name!='search':
                self.assoclist.place_forget()
        def fillsearch(event:tk.Event)->None:
            self.symbol=self.assoclist.get(self.assoclist.curselection()[0])[0]
            self.assoclist.place_forget()
            self.search.set('')
            self.fromp=3
            self.page.set(6)
            swcpage()
        def printaxis(event:tk.Event)->None:
            if event.x<144 or event.x>1368 or len(self.infolist)==0:return
            info=self.infolist.iloc[(event.x-144)*241//1224].values
            try:self.axis.config(text='m:%s   p:%.2f   avg_p:%.2f   v:%d'%tuple(info))
            except ValueError:self.axis.config(text='m:%s   p:-   avg_p:-   v:-'%info[0])
        def gettradepage()->tuple[int,int]:
            buff=try_get('\x03ct '+self.user,self.tradehint).split(' ')
            if buff[0]=='\x03ct':return ((int(buff[1])-1)//5+1,(int(buff[2])-1)//5+1)
            else:return (1,1)
        def fillform()->None:
            form(self.user,self.sock).main.transient()
        def turnhpage(dir:int)->None:
            newpage=(self.hpage+dir-1)%self.hmaxpage+1
            if newpage==self.hpage:return
            self.hpage=newpage
            togglegrab()
        def turnopage(dir:int)->None:
            newpage=(self.opage+dir-1)%self.omaxpage+1
            if newpage==self.opage:return
            self.opage=newpage
            togglegrab()
        def histclick(event:tk.Event)->None:
            if self.page.get()!=4:return
            name=event.widget.winfo_name()
            if name[:4]!='hist':return
            index=int(name[4:])
            if index<10:return
            if self.histsel==index//10:return
            index-=index%10
            for i in range(self.histsel*10+1,self.histsel*10+10):
                self.history[i].config(bg='#000000')
            for i in range(index+1,index+10):
                self.history[i].config(bg='#000099')
            self.histsel=index//10
        def askcancel()->None:
            aid=self.history[self.histsel*10+1].cget('text')
            state=self.history[self.histsel*10+9].cget('text')
            if aid=='' or state!='未成交':return
            if not messagebox.askyesno('确认','确定撤销该委托吗？',parent=self.main):return
            buff=try_get('\x03cancel '+aid,self.tradehint).split(' ')
            if buff[0]=='\x03cancel':self.tradehint.config(text='取消成功')
            elif buff[0]=='\x03null':self.tradehint.config(text='取消失败')
            togglegrab()
        # mainwin
        self.main=tk.Tk()
        self.main.title('stocksim')
        self.main.geometry('1280x720')
        self.main.minsize(1280,720)
        self.main.state('zoomed')
        self.main.config(bg='#000000')
        self.main.protocol('WM_DELETE_WINDOW',exitTk)
        self.main.bind('<MouseWheel>',scroll)
        self.main.bind('<Button-1>',cellclick)
        self.main.bind('<Button-1>',hideassoc,add='+')
        self.main.bind('<Button-1>',histclick,add='+')
        self.main.bind('<Double-Button-1>',cellselect)
        # global util
        self.frame:list[tk.Frame]=[tk.Frame(self.main,bg='#000000') for _ in range(7)]
        self.needgrab:bool=False
        self.alive:bool=True
        self.lock=threading.Lock()
        self.cond=threading.Condition(self.lock)
        self.timer=threading.Thread(target=timer,daemon=True)
        self.grabber=threading.Thread(target=grabber,daemon=True)
        self.tarpage:int=0
        self.uid:str=''
        self.sock=socket.socket(socket.AF_INET6,socket.SOCK_DGRAM)
        try_bind()
        # topbar
        self.page=tk.IntVar(self.main,0)
        self.topbtn:list[tk.Radiobutton]=[]
        for val,txt in zip(range(6),('行情','资讯','用户信息','我的自选','模拟交易','登录')):
            self.topbtn.append(tk.Radiobutton(self.main,font=('YaHei',14),text=txt,selectcolor='#000099',value=val,variable=self.page,bg='#000000',fg='#FFFF00',offrelief='flat',overrelief='groove',relief='sunken',indicatoron=False,command=swcpage))
        for radbtn,xpos in zip(self.topbtn[:2]+[self.topbtn[-1]],(0,200,400)):
            radbtn.place(x=xpos,y=0,width=200,height=30)
        # mkt
        self.frame[0].place(x=0,y=40,relwidth=1,relheight=1,height=-40)
        self.node=tk.StringVar(self.frame[0],'hs_a')
        for i,val,txt in zip(range(3),('hs_a','hs_b','hs_s'),('沪深京A','沪深京B','指数')):
            tk.Radiobutton(self.frame[0],font=('YaHei',12),text=txt,highlightbackground='#000000',border=3,selectcolor='#000000',fg='#FFFFFF',bg='#000000',value=val,variable=self.node,indicatoron=False,offrelief='flat',overrelief='flat',relief='sunken',command=swcnode).place(x=150*i,y=0,width=150,height=30)
        self.table=tk.Frame(self.main,bg='#000000')
        self.cell:list[tk.Label]=[]
        self.asc:int=0
        self.select:int=1
        self.sort=('symbol','name','trade','pricechange','changepercent','buy','sell','settlement','open','high','low','volume','amount','pb','mktcap','nmc','turnoverratio')
        self.currsort:int=0
        self.tpage:int=1
        self.maxpage:int=tools.getmaxpage('hs_a')
        align=(0,100,200,300,400,500,600,700,800,900,1000,1100,1200,1320,1440,1520,1660,1800,1880)
        for i in range(26):
            for j in range(18):
                tlabel=tk.Label(self.table,font=('YaHei',12),anchor='e',fg='#FFFFFF',bg='#000000',name='cell%d'%(i*18+j))
                tlabel.place(x=align[j],y=35*i,width=align[j+1]-align[j],height=35)
                self.cell.append(tlabel)
        for i in range(19,452,18):
            self.cell[i].config(fg='#FFFF00')
            self.cell[i+1].config(fg='#FFFF00')
        for i,name in enumerate(('代码','名称','现价','涨跌额','涨跌幅%','买入','卖出','昨收','今开','最高','最低','成交量/手','成交额/万','市净率','市值','流通值','换手率')):
            self.cell[i+1].config(text=name)
        self.table.place(x=0,y=80,relwidth=1,relheight=1,height=-80)
        # detail
        self.frame[6].place(x=0,y=40,relwidth=1,relheight=1,height=-40)
        self.symbol:str=''
        self.title=tk.Label(self.frame[6],font=('YaHei',24),anchor='w',fg='#FFFF00',bg='#000000')
        self.title.place(x=50,y=20,width=200,height=50)
        self.titlep=tk.Label(self.frame[6],font=('YaHei',24),anchor='w',fg='#FFFFFF',bg='#000000')
        self.titlep.place(x=250,y=20,width=200,height=50)
        self.img:ImageTk.PhotoImage
        self.graph=tk.Canvas(self.frame[6],highlightthickness=3,bg='#000000')
        self.graph.place(x=50,y=70,width=1440,height=840)
        self.graph.bind('<Motion>',printaxis)
        self.graph.bind('<Leave>',lambda event:self.axis.config(text=''))
        self.detail:list[tk.Label]=[]
        self.io5p:list[tk.Label]=[]
        self.io5v:list[tk.Label]=[]
        self.fromp:int=0
        for i,name in enumerate(('昨收','今开','最高','最低','涨跌额','涨跌幅%','振幅%','成交量/手','成交额/万','总股本/万','流通股/万','总市值/亿','流通值/亿','换手率%')):
            tk.Label(self.frame[6],font=('YaHei',12),text=name,fg='#00FFFF',bg='#000000',anchor='w').place(x=-300,relx=1,y=i*30,width=90,height=30)
            tlabel=tk.Label(self.frame[6],font=('YaHei',12),fg='#FFFFFF',bg='#000000',anchor='e')
            tlabel.place(x=-210,relx=1,y=i*30,width=200,height=30)
            self.detail.append(tlabel)
        self.detail[2].config(fg='#FF0000')
        self.detail[3].config(fg='#00FF00')
        for i,name in enumerate(('卖5','卖4','卖3','卖2','卖1','成交','买1','买2','买3','买4','买5')):
            tk.Label(self.frame[6],font=('YaHei',12),text=name,anchor='w',fg='#FFFFFF',bg='#000000').place(x=-300,relx=1,y=450+i*40,width=40,height=40)
            tlabel=tk.Label(self.frame[6],font=('YaHei',12),anchor='center',fg='#FFFFFF',bg='#000000')
            tlabel.place(x=-260,relx=1,y=450+i*40,width=150,height=40)
            self.io5p.append(tlabel)
            tlabel=tk.Label(self.frame[6],font=('YaHei',12),anchor='e',fg='#FFFF00',bg='#000000')
            tlabel.place(x=-110,relx=1,y=450+i*40,width=100,height=40)
            self.io5v.append(tlabel)
        self.io5v=self.io5v[:5]+self.io5v[6:]+[self.io5v[5]]
        self.io5p=self.io5p[:5]+self.io5p[6:]+[self.io5p[5]]
        self.infolist=[]
        self.axis=tk.Label(self.frame[6],font=('YaHei',16),bg='#000000',fg='#FFFFFF',anchor='w')
        self.axis.place(x=50,y=910,width=1440,height=30)
        tk.Button(self.frame[6],font=('YaHei',16),text='返回',bg='#000000',fg='#FFFFFF',overrelief='groove',relief='flat',command=returnto).place(x=-420,relx=1,y=20,width=100,height=50)
        tk.Button(self.frame[6],font=('YaHei',16),text='添加自选',bg='#000000',fg='#00FFFF',relief='groove',command=lambda:chgchoice(self.symbol,1)).place(x=450,y=30,width=120,height=30)
        tk.Button(self.frame[6],font=('YaHei',16),text='移除自选',bg='#000000',fg='#00FFFF',relief='groove',command=lambda:chgchoice(self.symbol,0)).place(x=570,y=30,width=120,height=30)
        self.chshint=tk.Label(self.frame[6],font=('YaHei',12),bg='#000000',fg='#FFFF00',anchor='w',highlightthickness=3)
        self.chshint.place(x=700,y=30,width=500,height=30)
        # news
        ...
        # userinfo
        self.frame[2].place(x=0,y=40,relwidth=1,relheight=1,height=-40)
        for i,txt in enumerate(('用户id','邮箱','可用资金','密码','原密码')):
            tk.Label(self.frame[2],font=('YaHei',16),anchor='e',text=txt,bg='#000000',fg='#FFFFFF').place(x=-220,relx=0.45,rely=0.2+0.1*i,width=200,height=30)
        self.usrinfo:list[tk.Entry]=[]
        self.user:str=''
        self.email:str=''
        for i in range(5):
            tentry=tk.Entry(self.frame[2],font=('YaHei',16),bg='#FFFFFF',fg='#000000',disabledbackground='#000000',disabledforeground='#FFFFFF',state='disable')
            tentry.place(x=20,relx=0.45,rely=0.2+0.1*i,width=400,height=30)
            self.usrinfo.append(tentry)
        self.chgstat=tk.IntVar(self.main,0)
        self.usrinfo[3].config(state='normal',show='*')
        self.usrinfo[4].config(state='normal',show='*')
        tk.Checkbutton(self.frame[2],font=('YaHei',16),text='修改',variable=self.chgstat,command=swcstat,bg='#000000',fg='#FFFFFF',selectcolor='#505050',overrelief='groove',offrelief='flat',indicatoron=False).place(x=450,relx=0.45,rely=0.3,width=100,height=30),
        self.usrhint=tk.Label(self.frame[2],font=('YaHei',12),bg='#000000',fg='#FFFF00',anchor='center')
        self.usrhint.place(relx=0.1,rely=0.8,relwidth=0.8,height=30)
        tk.Button(self.frame[2],font=('YaHei',16),text='确认修改',bg='#000000',fg='#FFFFFF',command=askmodify).place(relx=0.4,rely=0.7,relwidth=0.2,height=40)
        tk.Button(self.frame[2],font=('YaHei',16),text='退出登录',bg='#000000',fg='#FFFF00',command=logout).place(relx=0.4,rely=0.7,y=50,relwidth=0.2,height=40)
        tk.Button(self.usrinfo[3],font=('YaHei',16),text='*',bg='#FFFFFF',fg='#000000',command=lambda:swcshow(self.usrinfo[3])).place(x=-30,relx=1,y=2,width=26,height=26)
        tk.Button(self.usrinfo[4],font=('YaHei',16),text='*',bg='#FFFFFF',fg='#000000',command=lambda:swcshow(self.usrinfo[4])).place(x=-30,relx=1,y=2,width=26,height=26)
        # choice
        self.frame[3].place(x=0,y=40,relwidth=1,relheight=1,height=-40)
        self.choice:list[str]=[]
        self.myhint=tk.Label(self.frame[3],font=('YaHei',12),bg='#000000',fg='#FFFFFF')
        self.myhint.place(x=400,y=10,width=500,height=30)
        self.search=tk.StringVar(self.main,'')
        tk.Entry(self.frame[3],insertbackground='#FFFFFF',font=('YaHei',12),textvariable=self.search,bg='#000000',fg='#FFFFFF',name='search').place(x=100,y=10,width=300,height=30)
        self.search.trace_add('write',showassoc)
        self.assoclist=tk.Listbox(self.main,font=('YaHei',12),height=5,name='assoc')
        self.assoclist.bind('<<ListboxSelect>>',fillsearch)
        # trade
        self.frame[4].place(x=0,y=40,relwidth=1,relheight=1,height=-40)
        tk.Label(self.frame[4],font=('YaHei',20),text='历史委托',bg='#000000',fg='#FFFFFF',anchor='w').place(x=50,y=20,width=200,height=50)
        tk.Button(self.frame[4],font=('YaHei',12),text='刷新',bg='#000000',fg='#FFFF00',overrelief='groove',relief='flat',command=togglegrab).place(x=250,y=30,width=100,height=30)
        tk.Button(self.frame[4],font=('YaHei',12),text='取消委托',bg='#000000',fg='#FFFF00',overrelief='groove',relief='flat',command=askcancel).place(x=370,y=30,width=100,height=30)
        tk.Label(self.frame[4],font=('YaHei',20),text='持仓',bg='#000000',fg='#FFFFFF',anchor='w').place(x=50,y=500,width=200,height=50)
        tk.Button(self.frame[4],font=('YaHei',20),text='提交委托',bg='#000000',fg='#00FFFF',overrelief='groove',relief='flat',command=fillform).place(x=-250,relx=1,y=20,width=200,height=50)
        tk.Button(self.frame[4],font=('YaHei',16),text='<',bg='#000000',fg='#FFFFFF',relief='groove',command=lambda:turnhpage(-1)).place(x=800,y=350,width=30,height=30)
        tk.Button(self.frame[4],font=('YaHei',16),text='>',bg='#000000',fg='#FFFFFF',relief='groove',command=lambda:turnhpage(1)).place(x=900,y=350,width=30,height=30)
        tk.Button(self.frame[4],font=('YaHei',16),text='<',bg='#000000',fg='#FFFFFF',relief='groove',command=lambda:turnopage(-1)).place(x=700,y=830,width=30,height=30)
        tk.Button(self.frame[4],font=('YaHei',16),text='>',bg='#000000',fg='#FFFFFF',relief='groove',command=lambda:turnopage(1)).place(x=800,y=830,width=30,height=30)
        self.tradehint=tk.Label(self.frame[4],font=('YaHei',12),bg='#000000',fg='#FFFF00',anchor='w')
        self.tradehint.place(x=500,y=30,width=500,height=30)
        self.history:list[tk.Label]=[]
        self.hpage:int=1
        self.hmaxpage:int=1
        self.histsel:int=1
        self.own:list[tk.Label]=[]
        self.opage:int=1
        self.omaxpage:int=1
        for i in range(6):
            for j in range(10):
                tlabel=tk.Label(self.frame[4],font=('YaHei',12),anchor='e',fg='#FFFFFF',bg='#000000',name='hist%d'%(i*10+j))
                tlabel.place(x=j*150,y=35*i+90,width=150,height=35)
                self.history.append(tlabel)
        for i in range(6):
            for j in range(9):
                tlabel=tk.Label(self.frame[4],font=('YaHei',12),anchor='e',fg='#FFFFFF',bg='#000000')
                tlabel.place(x=j*150,y=35*i+570,width=150,height=35)
                self.own.append(tlabel)
        for i,name in enumerate(('委托单号','股票代码','买入/卖出','目标价格','成交价格','委托数量','委托日期','成交日期','委托状态'),start=1):
            self.history[i].config(text=name)
        for i,name in enumerate(('股票代码','股票名称','持仓数量','平均成本','市价','市值','浮动盈亏','盈亏率%'),start=1):
            self.own[i].config(text=name)
        for i in range(11,20):self.history[i].config(bg='#000099')
        # login
        self.frame[5].place(x=0,y=40,relwidth=1,relheight=1,height=-40)
        tk.Label(self.frame[5],font=('YaHei',16),anchor='e',text='用户id/邮箱',bg='#000000',fg='#FFFFFF').place(x=-220,relx=0.45,rely=0.4,width=200,height=30)
        tk.Label(self.frame[5],font=('YaHei',16),anchor='e',text='密码',bg='#000000',fg='#FFFFFF').place(x=-220,relx=0.45,y=50,rely=0.4,width=200,height=30)
        self.user_in=tk.Entry(self.frame[5],font=('YaHei',14))
        self.user_in.place(x=20,relx=0.45,rely=0.4,width=300,height=30)
        self.psw_in=tk.Entry(self.frame[5],font=('YaHei',14),show='*')
        self.psw_in.place(x=20,relx=0.45,y=50,rely=0.4,width=300,height=30)
        tk.Button(self.psw_in,font=('YaHei',16),text='*',bg='#FFFFFF',fg='#000000',command=lambda:swcshow(self.psw_in)).place(x=-30,relx=1,y=2,width=26,height=26)
        tk.Button(self.frame[5],font=('YaHei',16),text='登录',bg='#000000',fg='#FFFFFF',command=asklog).place(relx=0.4,rely=0.6,relwidth=0.2,height=40)
        tk.Button(self.frame[5],font=('YaHei',16),text='注册',bg='#000000',fg='#FFFFFF',command=askreg).place(relx=0.4,rely=0.65,relwidth=0.2,height=40)
        self.loghint=tk.Label(self.frame[5],font=('YaHei',12),bg='#000000',fg='#FFFF00')
        self.loghint.place(relx=0.1,rely=0.8,relwidth=0.8,height=40)
        # start
        self.timer.start()
        self.grabber.start()
        swcpage()
socket.setdefaulttimeout(2)
client().main.mainloop()