import httpx
import json
import matplotlib.pyplot as plt
import matplotlib
from PIL import Image
import pandas as pd
import numpy as np
import time
import re
matplotlib.use('Agg')
def itos_pre0(num:int,length:int)->str:
    res=str(num)
    return '0'*(length-len(res))+res
ticks:pd.DataFrame=pd.DataFrame([
    '09:%s:00'%(itos_pre0(m,2)) for m in range(30,60)]+[
    '10:%s:00'%(itos_pre0(m,2)) for m in range(60)]+[
    '11:%s:00'%(itos_pre0(m,2)) for m in range(31)]+[
    '%s:%s:00'%(itos_pre0(h,2),itos_pre0(m,2)) for h in range(13,15) for m in range(60)]+[
    '15:00:00'],
    columns=['m']
)
ticks.drop(ticks[ticks['m']=='13:00:00'].index,inplace=True)
def voidcall(args=None)->list:
    return []
def getshtml(url:str)->str:
    return httpx.get(
        url=url,
        headers={
            'User-Agent':'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/123.0.0.0 Safari/537.36',
            'Referer':'https://finance.sina.com.cn/'
        },
        timeout=2
    ).text
def getmaxpage(node:str)->int:
    try:return int(getshtml(
        'http://vip.stock.finance.sina.com.cn/quotes_service/api/json_v2.php/Market_Center.getHQNodeStockCount?node='+node
    ).strip('"'))//25+1
    except ValueError:
        return 1
def dicttolist(origin:dict)->list[str]:
    del origin['code'],origin['ticktime'],origin['per']
    res=list(origin.values())
    for i,item in enumerate(res[2:],start=2):
        res[i]='%.2f'%float(item)
    res[11]=res[11][:-5]
    res[12]='%.2f'%(float(res[12])/10000)
    for indx in [5,6,13,14,15,16]:
        if res[indx]=='0.00':res[indx]='-'
    return res
def getmktinfo(page:int,sort:str,asc:int,node:str)->list:
    try:data:list[dict]=json.loads(getshtml(
        'http://vip.stock.finance.sina.com.cn/quotes_service/api/json_v2.php/Market_Center.getHQNodeData?page=%d&num=25&sort=%s&asc=%d&node=%s'\
        %(page,sort,asc,node)
    ))
    except json.JSONDecodeError:
        return []
    for i,infodict in enumerate(data):
        data[i]=dicttolist(infodict)
    return data
def getstkinfo(symbol_list:list[str],simple:bool=False)->list[list[str]]:
    data=getshtml(
        'http://hq.sinajs.cn/etag.php?_=%d&list=%s'\
        %(time.time_ns()//1000000,','.join(symbol_list+[symbol+'_i' for symbol in symbol_list]))
    ).split(';\n')
    gap:int=len(data)//2
    res:list[list[str]]=[]
    for i in range(gap):
        info=data[i][data[i].find('"')+1:-2].split(',')
        info_i=data[i+gap][data[i+gap].find('"')+1:-1].split(',')
        if len(info)<2:continue
        if simple:
            res.append(
                [symbol_list[i],info[0]]+[
                    '%.2f'%item for item in [
                        float(info[3]),(float(info[3])-float(info[2])),
                        (100*(float(info[3])/float(info[2])-1)),
                    ]
                ]+([
                    '%.2f'%float(info[11]),'%.2f'%float(info[21])
                ] if info_i[0]!='I' else [
                    '-','-'
                ])+[
                    '%.2f'%item for item in [
                        float(info[2]),float(info[1]),
                        float(info[4]),float(info[5]),
                    ]
                ]+([
                    info[8][:-2],'%.2f'%(float(info[9])/10000),
                    '-',
                    '%.2f'%(float(info[3])*float(info_i[7])),
                    '%.2f'%(float(info[3])*float(info_i[8])),
                    '%.2f'%(float(info[8])/(float(info_i[8])*100))
                ] if info_i[0]!='I' else [
                    info[8],'%.2f'%(float(info[9])/10000),
                    '-','-','-','-'
                ])
            )
        else:
            res.append(
                [info[0]]+
                [
                    '%.2f'%float(item)
                        for item in [info[3],info[2],info[1],info[4],info[5]]
                ]+
                [
                    '%.2f'%(float(info[3])-float(info[2])),
                    '%.2f'%(100*(float(info[3])/float(info[2])-1)),
                    '%.2f'%(100*((float(info[4])-float(info[5])))/float(info[2])),
                    info[8] if info_i[0]=='I' else info[8][:-2],
                    '%.2f'%(float(info[9])/10000),
                ]+
                (
                    ['-']*25 if info_i[0]=='I' else [
                        info_i[7],info_i[8],
                        '%.2f'%(float(info[3])*float(info_i[7])/10000),
                        '%.2f'%(float(info[3])*float(info_i[8])/10000),
                        '%.2f'%(float(info[8])/(float(info_i[8])*100)),
                    ]+
                    [info[i] for i in range(28,18,-2)]+[info[i] for i in range(10,20,2)]+
                    ['%.2f'%float(info[i]) for i in range(29,19,-2)]+
                    ['%.2f'%float(info[i]) for i in range(11,21,2)]
                )
            )
    return res
def getminline(symbol:str)->pd.DataFrame:
    try:
        data=pd.DataFrame(json.loads(getshtml(
            'http://cn.finance.sina.com.cn/minline/getMinlineData?version=7.11.0&symbol='+symbol
        ))['result']['data'],columns=['m','p','avg_p','v'])
    except json.JSONDecodeError:data=pd.DataFrame()
    except KeyError:data=pd.DataFrame()
    if len(data)==0:return data
    data=pd.merge(data,ticks[ticks['m']<data.loc[len(data)-1,'m']],how='outer',on='m',sort='m').reset_index(drop=True)
    data['p']=data['p'].ffill().astype(float)
    data['avg_p']=data['avg_p'].ffill().astype(float)
    data['v']=data['v'].fillna('0').astype(int)//100
    data=pd.merge(data,ticks,how='outer',on='m')
    data['m']=data['m'].str[:-3]
    return data
def drawtoday(data:pd.DataFrame,cls:float,tmax:float,tmin:float):
    fig,(liner,boxer)=plt.subplots(2)
    fig.set_facecolor('#000000')
    fig.set_size_inches(14.4,8.4)
    liner.set_facecolor('#000000')
    boxer.set_facecolor('#000000')
    liner.plot(data['m'],data['p'])
    liner.plot(data['m'],data['avg_p'],color='magenta')
    liner.set_xticks(['09:30','10:30','11:30','14:00','15:00'])
    liner.set_xlim('09:30','15:30')
    gap=max(tmax-cls,cls-tmin)
    liner.set_yticks([round(num,2) for num in np.linspace(cls-gap,cls+gap,7)])
    clist=['#FF0000' if delta>0 else '#00FF00' for delta in data['p'].diff(1)[1:]]
    boxer.bar(data['m'][1:],data['p'].diff(1).abs()[1:],color=clist)
    liner.grid(which='major',color='#FFFFFF',linestyle=':')
    liner.tick_params('both',labelcolor='#FFFFFF')
    liner.set_position((0.1,0.2,0.85,0.7))
    boxer.set_position((0.1,0,0.85,0.15))
    fig.canvas.draw()
    return Image.frombytes('RGB',fig.canvas.get_width_height(),fig.canvas.tostring_rgb())
def checkinfo(email:str,psw:str)->int:
    if re.fullmatch('[\w]+@[a-z]+(\.[a-z]+)+',email) is None:return 1
    if re.fullmatch('[\w -]*?',psw) is None:return 2
    return 0
def getassoc(currword:str)->list[str]:
    data=getshtml(
        'http://suggest3.sinajs.cn/suggest/type=11,12,13,14,15&key=%s&name=suggestdata_%d&num=5'%(currword,time.time_ns()//1000000)
    )
    data=data[data.find('"')+1:-2].split(';')
    return [item.split(',')[3:5] for item in data]
def getstksimple(info_list:list[list[str]])->list[list[str]]:
    data=getshtml(
        'http://hq.sinajs.cn/etag.php?_=%d&list=%s'%\
        (time.time_ns()//1000000,','.join([info[0] for info in info_list]))
    ).split(';\n')[:-1]
    res=[]
    for i in range(len(data)):
        info=data[i][data[i].find('"')+1:-2].split(',')
        avgcost=float(info_list[i][2])/int(info_list[i][1])
        amount=int(info_list[i][1])
        res.append(
            [
                info_list[i][0],info[0],info_list[i][1],
                '%.3f'%avgcost,'%.2f'%float(info[3]),
                '%.2f'%(float(info[3])*amount),
                '%.2f'%((float(info[3])-avgcost)*amount),
                '%.2f'%((float(info[3])-avgcost)*amount*100/float(info_list[i][2]))
            ]
        )
    return res
if __name__=='__main__':
    # print(getstksimple([['sh600073','2000','12962']]))
    print(getassoc('sh6005'))
    # drawtoday('sh600519',float(info[2]),float(info[4]),float(info[5])).show()