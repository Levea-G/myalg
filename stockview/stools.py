import httpx
import re
import time
def cutlist(data:list,length:int)->list[list]:
    return [data[i:i+length] for i in range(0,len(data),length)]
def getprice(symbol_list:list[str])->list[float]:
    res=[]
    for slice in cutlist(symbol_list,100):
        tdata=httpx.get(
            url='http://hq.sinajs.cn/etag.php?_=%d&list=%s'\
            %(time.time_ns()//1000000,','.join(slice)),
            headers={
                'User-Agent':'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/125.0.0.0 Safari/537.36',
                'Referer':'https://finance.sina.com.cn'            
            }
        ).text.split(';\n')[:-1]
        for info in tdata:
            info=info[info.find('"')+1:-2].split(',')
            res.append(round(float(info[3]),2))
    return res
if __name__=='__main__':
    print(getprice(['sh600519','sh000001']))