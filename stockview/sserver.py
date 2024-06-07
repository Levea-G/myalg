import socket
import sqlite3 as sql
import threading
import queue
import time
import stools
def gettime_hms()->str:
    '''return timestamp string'''
    return time.strftime('[%Y-%m-%d  %H:%M:%S]',time.localtime(time.time()))
def gettime_ymd()->str:
    return time.strftime('%Y-%m-%d',time.localtime(time.time()))
def gettime_hex()->str:
    '''return hex time'''
    return hex(int(time.time()))[2:]
class sserver:
    def __init__(self)->None:
        def checkdb()->None:
            conn=sql.connect('record.crv')
            cur=conn.cursor()
            try:
                cur.execute(
                    '''
                    create table user(
                        uid int,
                        email char(30) unique,
                        password char(20),
                        cash decimal(12,2),
                        primary key(uid)
                    )
                    '''
                )
            except sql.OperationalError:pass
            try:
                cur.execute(
                    '''
                    create table auth(
                        aid char(30),
                        uid char(15),
                        symbol char(8),
                        dir bit,
                        tarprice decimal(7,2),
                        tradeprice decimal(7,2),
                        tradetime char(10),
                        amount int,
                        authtime char(10),
                        state int,
                        primary key(aid),
                        foreign key(uid) references user(uid)
                    )
                    '''
                )
            except sql.OperationalError:pass
            try:
                cur.execute(
                    '''
                    create table choice(
                        uid char(15),
                        symbol char(8),
                        primary key(uid,symbol),
                        foreign key(uid) references user(uid)
                    )
                    '''
                )
            except sql.OperationalError:pass
            try:
                cur.execute(
                    '''
                    create table owns(
                        uid char(15),
                        symbol char(8),
                        ownamount int,
                        cost decimal(10,2),
                        primary key(uid,symbol),
                        foreign key(uid) references user(uid)
                    )
                    '''
                )
            except sql.OperationalError:pass
            cur.close()
            conn.close()
        self.host:str=socket.getaddrinfo(socket.gethostname(),None,socket.AF_INET6)[-1][-1][0]
        self.port:int=1112
        self.sock=socket.socket(socket.AF_INET6,socket.SOCK_DGRAM)
        self.sock.bind((self.host,self.port))
        self.sock.settimeout(2)
        self.runing:bool=True
        print('socket initialize done')
        checkdb()
        print('database checkup done')
        self.recver=threading.Thread(target=self.recv,daemon=True)
        self.sender=threading.Thread(target=self.send,daemon=True)
        self.trader=threading.Thread(target=self.trade,daemon=True)
        self.timer=threading.Event()
        self.worker:list[threading.Thread]=[threading.Thread(target=self.work,daemon=True) for _ in range(10)]
        self.recvq=queue.Queue(100000)
        self.sendq=queue.Queue(100000)
        self.recver.start()
        print('receiver start')
        self.sender.start()
        print('sender start')
        for thread in self.worker:
            thread.start()
        print('workers start')
        self.trader.start()
        print('trader start')
        print('%sServer\nServer started on %s:%d\n'%(gettime_hms(),self.host,self.port))
    def recv(self)->None:
        while self.runing:
            try:buffer,addr=self.sock.recvfrom(128)
            except socket.timeout:continue
            try:self.recvq.put((buffer.decode(),addr[:2]),timeout=1)
            except queue.Full:print(gettime_hms()+'Server\nServer task queue full!\n')
    def send(self)->None:
        while True:
            info:tuple[str,tuple]=self.sendq.get()
            if not self.runing:break
            self.sock.sendto(info[0].encode(),info[1])
    def work(self)->None:
        while True:
            info:tuple[str]=self.recvq.get()
            if not self.runing:break
            self.proc(info[0],info[1])
    def trade(self)->None:
        while self.runing:
            a=time.time()
            conn=sql.connect('record.crv')
            cur=conn.cursor()
            cur.execute('select symbol from auth where state=0 group by symbol')
            symbol_list=[item[0] for item in cur.fetchall()]
            cur.close()
            conn.close()
            price_list=stools.getprice(symbol_list)
            conn=sql.connect('record.crv')
            cur=conn.cursor()
            for symbol,price in zip(symbol_list,price_list):
                cur.execute('select aid,tarprice,dir,uid,amount from auth where symbol=? and state=0',(symbol,))
                for auth in cur.fetchall():
                    cur.execute('update auth set state=1,tradeprice=?,tradetime=? where aid=?',(price,gettime_ymd(),auth[0]))
                    if auth[2] and auth[1]>=price:
                        cur.execute('select ownamount from owns where uid=? and symbol=?',(auth[3],symbol))
                        amount=cur.fetchone()
                        if amount is None:
                            cur.execute('insert into owns(uid,symbol,ownamount,cost) values(?,?,?,?)',(auth[3],symbol,auth[4],auth[1]*auth[4]))
                        else:
                            cur.execute('update owns set ownamount=ownamount+?,cost=cost+? where uid=? and symbol=?',(auth[4],auth[1]*auth[4],auth[3],symbol))
                    elif not auth[2] and auth[1]<=price:
                        cur.execute('update user set cash=cash+? where uid=?',(auth[1]*auth[4],auth[3]))
            conn.commit()
            cur.close()
            conn.close()
            b=time.time()
            print(gettime_hms()+'Server:\ntraded\n')
            self.timer.wait(30+a-b)
    def proc(self,buff:str,addr:tuple[str,int])->None:
        print(gettime_hms()+addr[0]+':%d'%addr[1]+'\n'+buff+'\n')
        buff=buff.split(' ')
        if buff[0]=='\x00log':
            conn=sql.connect('record.crv')
            cur=conn.cursor()
            cur.execute('select uid from user where uid=? or email=? and password=?',(buff[1],buff[1],buff[2]))
            uid=cur.fetchone()
            cur.close()
            conn.close()
            if uid is None:response='\x00forbid'
            else:response='\x00permit %d'%uid[0]
        elif buff[0]=='\x00reg':
            conn=sql.connect('record.crv')
            cur=conn.cursor()
            cur.execute('select uid from user where email=?',(buff[1],))
            if cur.fetchone() is None:
                cur.execute('select count(uid) from user')
                uid=cur.fetchone()[0]+1
                cur.execute('insert into user(uid,email,password,cash) values(?,?,?,?)',(uid,buff[1],buff[2],'1000000.00'))
                conn.commit()
                response='\x00permit %d'%uid
            else:response='\x00repeat'
            cur.close()
            conn.close()
        elif buff[0]=='\x01get':
            conn=sql.connect('record.crv')
            cur=conn.cursor()
            cur.execute('select uid,email,cash from user where uid=?',(int(buff[1]),))
            info=cur.fetchone()
            cur.close()
            conn.close()
            if info is not None:
                response='\x01got %d %s %.2f'%info
            else:response='\x01null'
        elif buff[0]=='\x01set':
            conn=sql.connect('record.crv')
            cur=conn.cursor()
            try:
                if buff[4]!='':cur.execute('update user set password=? and email=? where uid=? and password=?',(buff[4],buff[2],int(buff[1]),buff[5]))
                else:cur.execute('update user set email=? where uid=? and password=?',(buff[2],int(buff[1]),buff[5]))
                conn.commit()
                if cur.rowcount==0:response='\x01psw'
                elif cur.rowcount==1:response='\x01set'
                else:response='\x01error'
            except sql.IntegrityError:response='\x01repeat'
            sql.IntegrityError
            cur.close()
            conn.close()
        elif buff[0]=='\x02ct':
            conn=sql.connect('record.crv')
            cur=conn.cursor()
            cur.execute('select count(symbol) from choice where uid=?',(buff[1],))
            info=cur.fetchone()
            cur.close()
            conn.close()
            if info is None:response='\x02null'
            else:response='\x02ct %d'%info[0]
        elif buff[0]=='\x02get':
            conn=sql.connect('record.crv')
            cur=conn.cursor()
            cur.execute('select symbol from choice where uid=? limit ?,25',(buff[1],int(buff[2])*25-25))
            info=cur.fetchall()
            cur.close()
            conn.close()
            response='\x02got '+','.join([item[0] for item in info])
        elif buff[0]=='\x02add':
            conn=sql.connect('record.crv')
            cur=conn.cursor()
            try:cur.execute('insert into choice(uid,symbol) values(?,?)',(int(buff[1]),buff[2]))
            except sql.IntegrityError:pass
            conn.commit()
            if cur.rowcount==1:response='\x02add'
            elif cur.rowcount==-1:response='\x02has'
            else:response='\x02eror'
            cur.close()
            conn.close()
        elif buff[0]=='\x02del':
            conn=sql.connect('record.crv')
            cur=conn.cursor()
            cur.execute('delete from choice where uid=? and symbol=?',(int(buff[1]),buff[2]))
            conn.commit()
            if cur.rowcount==1:response='\x02del'
            elif cur.rowcount==0:response='\x02null'
            else:response='\x02eror'
            cur.close()
            conn.close()
        elif buff[0]=='\x03get':
            conn=sql.connect('record.crv')
            cur=conn.cursor()
            cur.execute('select aid,symbol,dir,tarprice,tradeprice,amount,authtime,tradetime,state from auth where uid=? limit ?,5',(int(buff[1]),int(buff[2])*5))
            info=[cur.fetchall()]
            cur.execute('select symbol,ownamount,cost from owns where uid=? and ownamount>0 limit ?,5',(int(buff[1]),int(buff[3])*5))
            info.append(cur.fetchall())
            cur.close()
            conn.close()
            response='\x03got '+';'.join([','.join([str(it) for it in item]) for item in info[0]])+' '+';'.join([','.join([str(it) for it in item]) for item in info[1]])
        elif buff[0]=='\x03ct':
            conn=sql.connect('record.crv')
            cur=conn.cursor()
            cur.execute('select count(aid) from auth where uid=?',(buff[1],))
            info=[cur.fetchone()]
            cur.execute('select count(symbol) from owns where uid=?',(buff[1],))
            info.append(cur.fetchone())
            cur.close()
            conn.close()
            if info[0] is None or info[1] is None:response='\x03null'
            else:response='\x03ct %d %d'%(info[0][0],info[1][0])
        elif buff[0]=='\x03add':
            conn=sql.connect('record.crv')
            cur=conn.cursor()
            response=''
            if int(buff[5])==0:
                cur.execute('select ownamount from owns where uid=? and symbol=?',(buff[1],buff[2]))
                try:own=int(cur.fetchone()[0])-int(buff[4])
                except TypeError:response='\x03null'
                if own<0:response='\x03noown'
            else:
                cur.execute('select cash from user where uid=?',(buff[1],))
                try:cash=float(cur.fetchone()[0])-int(buff[4])*float(buff[3])
                except TypeError:response='\x03null'
                if cash<0:response='\x03nocash'
            if response=='':
                try:
                    cur.execute(
                        '''
                        insert into auth(aid,uid,symbol,tarprice,tradeprice,authtime,tradetime,amount,dir,state)
                        values(?,?,?,?,?,?,?,?,?,?)
                        ''',
                        (gettime_hex()+buff[1],buff[1],buff[2],buff[3],'0.00',gettime_ymd(),'-',int(buff[4]),int(buff[5]),0)
                    )
                    if int(buff[5])==1:
                        cur.execute(
                            '''
                            update user set cash=? where uid=?
                            ''',
                            ('%.2f'%cash,buff[1])
                        )
                    else:
                        cur.execute(
                            '''
                            update owns set ownamount=?,cost=cost-? where uid=? and symbol=?
                            ''',
                            (own,round(int(buff[4])*float(buff[3]),2),buff[1],buff[2])
                        )
                    conn.commit()
                    response='\x03add'
                except sql.IntegrityError:response='\x03freq'
            cur.close()
            conn.close()
        elif buff[0]=='\x03cancel':
            conn=sql.connect('record.crv')
            cur=conn.cursor()
            cur.execute('update auth set state=2 where aid=? and state=0',(buff[1],))
            if cur.rowcount==0:response='\x03null'
            else:
                cur.execute('select dir,amount,tarprice,uid,symbol from auth where aid=?',(buff[1],))
                info=cur.fetchone()
                if info[0]==1:
                    cur.execute('update user set cash=cash+? where uid=?',(info[1]*round(info[2],2),info[3]))
                else:
                    cur.execute('update owns set ownamount=ownamount+?,cost=cost+? where uid=? and symbol=?',(info[1],info[1]*round(float(info[2]),2),info[3],info[4]))
                if cur.rowcount==0:response='\x03null'
                elif cur.rowcount==1:response='\x03cancel'
            conn.commit()
            cur.close()
            conn.close()
        self.sendq.put((response,addr))
        print(response)
    def run(self)->None:
        while True:
            command=input().split(' ')
            if command[0]=='/quit':
                self.runing=False
                break
            elif command[0]=='/gen':
                try:self.recvq.put((command[1],command[2]),timeout=1)
                except queue.Full:print(gettime_hms()+'Server\nServer task queue full!\n')
                except IndexError:print(gettime_hms()+'Server\nCommand syntax error\n')
        for _ in range(10):self.recvq.put(('no_op',('Server',1112)))
        self.sendq.put(('no_op',('Server',1112)))
        self.timer.set()
        self.trader.join()
        print(gettime_hms()+'Server\ntrader exit')
        for i in range(10):self.worker[i].join()
        print(gettime_hms()+'Server\nworkers exit')
        self.sender.join()
        print(gettime_hms()+'Server\nsender exit')
        self.recver.join()
        print(gettime_hms()+'Server\nreceiver exit')
        self.sock.close()
        print(gettime_hms()+'Server\nServer closed')
if __name__=='__main__':
    try:sserver().run()
    except KeyboardInterrupt:print(gettime_hms()+'Server\nServer killed by interruption')
    # print(stools.cutlist([1,2,3,4,5,6,7],5))