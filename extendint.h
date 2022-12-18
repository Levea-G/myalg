//c99+ utf-8
#include<stdio.h>
#define ext struct extendint
/*
'maxlength' represents int length limit
while every 1024 digits take 1KB of storage
please be careful when modifying this
*/#define maxlength 4005
typedef int ll;
struct extendint{
    char s[maxlength];
    ll pst;
};
ext read_(){//read extendint
    ext t={{0},0};char c=getchar();ll f=1,k=0;
    while(c<'0'||c>'9'){if(c=='-')f=-1;c=getchar();}
    while(c>='0'&&c<='9'){t.s[++k]=c-48;c=getchar();}
    for(ll i=1,j=k;i<j;i++,j--)t.s[i]^=t.s[j]^=t.s[i]^=t.s[j];
    t.pst=k*f;return t;
}
void print_(ext t){//print extendint
    ll k=t.pst;
    if(k<0)putchar('-'),k=~k+1;
    while(k)putchar(t.s[k--]+48);
}
ext inverse_(ext t){//return -t
    ext tem=t;
    if(tem.s[1]||tem.pst>1)tem.pst=~tem.pst+1;
    return tem;
}
ll cmp_(ext a,ext b){//return a<b:0 a>b:1 a==b:2 as int
    if(a.pst!=b.pst)return a.pst>b.pst;
    ll t=a.pst>0?a.pst:-a.pst;
    while(a.s[t]==b.s[t]&&t)t--;
    if(!t)return 2;
    return (a.s[t]>b.s[t])^(a.pst<0);
}
ext trans_(long long t){//return t as extendint
    ext tem={{0},0};ll k=0;
    if(!t){tem.pst=1;return tem;}
    while(t)tem.s[++k]=t%10,t/=10;
    tem.pst=t<0?-k:k;
    return tem;
}
ext add_(ext a,ext b){//return a+b
    ext t={{0},0};ll k1=a.pst,k2=b.pst;
    if(k1<0){k1=~k1+1;for(ll i=k1;i;i--)a.s[i]=~a.s[i]+1;}
    if(k2<0){k2=~k2+1;for(ll i=k2;i;i--)b.s[i]=~b.s[i]+1;}
    ll k=k1>k2?k1:k2,f=1;
    for(ll i=1;i<=k;i++)t.s[i]=a.s[i]+b.s[i];
    while(!t.s[k]&&k>1)k--;
    if(t.s[k]<0){
        t.s[k]=~t.s[k],f=-1,t.s[1]--;
        for(ll i=k-1;i;i--)t.s[i]=9-t.s[i];
    }
    for(ll i=1;i<=k;i++){
        if(t.s[i]>9)t.s[i]-=10,t.s[i+1]++;
        else if(t.s[i]<0)t.s[i]+=10,t.s[i+1]--;
    }
    for(k++;!t.s[k]&&k>1;k--);t.pst=k*f;
    return t;
}
ext minus_(ext a,ext b){//return a-b
    return add_(a,inverse_(b));
}
ext mul_(ext a,ext b){//return a*b
    ext t={{0},0};ll k1=a.pst>0?a.pst:-a.pst,k2=b.pst>0?b.pst:-b.pst,k=k1+k2;
    for(ll i=1;i<=k1;i++)
        for(ll j=1;j<=k2;j++){
            t.s[i+j-1]+=a.s[i]*b.s[j];
            t.s[i+j]+=t.s[i+j-1]/10;
            t.s[i+j-1]%=10;
        }
    while(!t.s[k]&&k>1)k--;
    if((a.pst>0)^(b.pst>0)&&(t.s[1]||k>1))k=~k+1;
    t.pst=k;
    return t;
}
ext divassist(ext*a,ext b,ll k){//assist function for div_
    ext t={{0},0};
    if(!cmp_(*a,b)){t.pst=k-1;return t;}
    t=divassist(a,mul_(b,trans_(10)),k+1);
    while(cmp_(*a,b))*a=minus_(*a,b),t.s[k]++;
    return t;
}
ext div_(ext a,ext b){//return a//b
    return divassist(&a,b,1);
}
ext fpower_(ext a,ll b){//return a^b
    ext t=trans_(1);
    for(;b;a=mul_(a,a),b>>=1)if(b&2)t=mul_(t,a);
    return t;
}