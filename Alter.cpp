#include<iostream>
namespace _Alter{
template<class key>
class rbtree{
public:
    class Iterator{
    public:
        Iterator():x(0),tr(NULL){}
        Iterator(const rbtree&t,int x):x(x),tr(t.tr){}
        Iterator(const Iterator&b):x(b.x),tr(b.tr){}
        Iterator&operator=(const Iterator&b){x=b.x;tr=b.tr;return *this;}
        bool forward(){return __forward__();}
        bool backward(){return __backward__();}
        const key&getkey()const{return tr[x]._key;}
        key copykey()const{return tr[x]._key;}
        int getcount()const{return tr[x].ct;}
        Iterator&operator++(int){__forward__();return *this;}
        Iterator&operator--(int){__backward__();return *this;}
        Iterator operator+(int steps)const{
            Iterator temp(*this);
            for(int i=0;i<steps;i++)temp.__forward__();
            return temp;
        }
        Iterator operator-(int steps)const{
            Iterator temp(*this);
            for(int i=0;i<steps;i++)temp.__backward__();
            return temp;
        }
        Iterator&operator+=(int steps){for(int i=0;i<steps;i++)__forward__();return *this;}
        Iterator&operator-=(int steps){for(int i=0;i<steps;i++)__backward__();return *this;}
        bool operator==(const Iterator&b)const{return tr==b.tr&&x==b.x;}
        bool operator!=(const Iterator&b)const{return tr!=b.tr||x!=b.x;}
        bool empty()const{return tr==NULL;}
    private:
        int x;
        typename rbtree<key>::node*tr;
        bool __forward__(){
            if(tr[x].s[1]){
                x=tr[x].s[1];
                while(tr[x].s[0])x=tr[x].s[0];
            }
            else{
                bool j;
                do{j=tr[tr[x].f].s[1]==x;x=tr[x].f;}while(j&&x);
            }
            return x;
        }
        bool __backward__(){
            if(tr[x].s[0]){
                x=tr[x].s[0];
                while(tr[x].s[1])x=tr[x].s[1];
            }
            else{
                bool j;
                do{j=tr[tr[x].f].s[1]!=x;x=tr[x].f;}while(j&&x);
            }
            return x;
        }
    };
    rbtree(bool do_alloc=0,int(*compare)(const key&,const key&)=&__defaultcmp__){
        __cmp__=compare;
        if(do_alloc==0){tr=NULL;return;}
        tr=new node[4];
        used=1;size=4;
        tr->act=tr->ct=tr->rb=tr->f=tr->s[0]=tr->s[1]=0;
    }
    rbtree(const rbtree&b){
        used=b.used;size=b.size;
        tr=new node[size];
        for(int i=0;i<used;i++)tr[i]=b.tr[i];
    }
    rbtree(rbtree&&b){
        tr=b.tr;b.tr=NULL;
        used=b.used;size=b.size;
    }
    ~rbtree(){
        delete[] tr;tr=NULL;
    }
    bool insert(const key&v){return __insert__(std::forward<const key&>(v));}
    bool insert(key&&v){return __insert__(std::forward<key&&>(v));}
    bool remove(const key&v){return __delete__(v);}
    bool reinit(bool do_alloc=1){return __reinit__(do_alloc);}
    Iterator operator[](int index)const{return __kth__(index);}
    int find(const key&v)const{return __find__(v);}
    Iterator loc(const key&v)const{return __loc__(v);}
    Iterator lowerbound(const key&v)const{return __lowerbound__(v);}
    Iterator upperbound(const key&v)const{return __upperbound__(v);}
    Iterator begin()const{return __begin__();}
    Iterator end()const{return __end__();}
    bool operator<(const rbtree&b)const{
        if(tr==b.tr)return 0;
        bool conda=(tr==NULL),condb=(b.tr==NULL);
        if(conda!=condb)return conda<condb?0:1;
        Iterator ita=end(),itb=b.end();
        conda=ita.forward(),condb=itb.forward();
        while(conda&&condb){
            int to=__cmp__(ita.getkey(),itb.getkey());
            if(to==2)conda=ita.forward(),condb=itb.forward();
            else return to?0:1;
        }
        return conda==condb?0:condb;
    }
    bool operator>(const rbtree&b)const{
        if(tr==b.tr)return 0;
        bool conda=(tr==NULL),condb=(b.tr==NULL);
        if(conda!=condb)return conda<condb?1:0;
        Iterator ita=end(),itb=b.end();
        conda=ita.forward(),condb=itb.forward();
        while(conda&condb){
            int to=__cmp__(ita.getkey(),itb.getkey());
            if(to==2)conda=ita.forward(),condb=itb.forward();
            else return to?1:0;
        }
        return conda==condb?0:conda;
    }
    bool operator==(const rbtree&b)const{
        if(tr==b.tr)return 1;
        bool conda=(tr==NULL),condb=(b.tr==NULL);
        if(conda!=condb)return 0;
        Iterator ita=end(),itb=b.end();
        conda=ita.forward(),condb=itb.forward();
        while(conda&condb){
            if(__cmp__(ita.getkey(),itb.getkey())==2)
                conda=ita.forward(),condb=itb.forward();
            else return 0;
        }
        return conda==condb?1:0;
    }
    bool operator!=(const rbtree&b)const{
        if(tr==b.tr)return 0;
        bool conda=(tr==NULL),condb=(b.tr==NULL);
        if(conda!=condb)return 1;
        Iterator ita=end(),itb=b.end();
        conda=ita.forward(),condb=itb.forward();
        while(conda&condb){
            if(__cmp__(ita.getkey(),itb.getkey())==2)
                conda=ita.forward(),condb=itb.forward();
            else return 1;
        }
        return conda==condb?0:1;
    }
    rbtree&operator=(rbtree b){
        swap(*this,b);
        return *this;
    }
    friend void swap(rbtree&a,rbtree&b){
        using std::swap;
        swap(a.used,b.used);
        swap(a.size,b.size);
        swap(a.tr,b.tr);
    }
    void print(const char*sep="\n",const char*end="",void(*_print)(const key&)=NULL)const{
        Iterator itx=this->end();
        if(itx.forward()==0){std::cout<<end;return;}
        if(_print==NULL)_print=__defaultprint__;
        while(1){
            _print(itx.getkey());
            std::cout<<' '<<itx.getcount();
            if(itx.forward())std::cout<<sep;
            else {std::cout<<end;return;}
        }
    }
    int valuecount()const{return tr[tr[0].s[1]].act;}
    int keycount()const{return used-1;}
    int cmp(const key&a,const key&b){return __cmp__(a,b);}
    static int selfcmp(const rbtree&a,const rbtree&b){
        if(a.tr==b.tr)return 2;
        bool conda=(a.tr==NULL),condb=(b.tr==NULL);
        if(conda!=condb)return conda<condb?1:0;
        Iterator ita=a.end(),itb=b.end();
        conda=ita.forward(),condb=itb.forward();
        while(conda&condb){
            int to=a.__cmp__(ita.getkey(),itb.getkey());
            if(to==2)conda=ita.forward(),condb=itb.forward();
            else return to;
        }
        return conda==condb?2:conda>condb;
    }
private:
    class node{
    public:
        node(){}
        void operator=(const node&b){
            f=b.f;s[0]=b.s[0];s[1]=b.s[1];
            ct=b.ct;act=b.act;
            rb=b.rb;
            _key=b._key;
        }
        void operator=(node&&b){
            f=b.f;s[0]=b.s[0];s[1]=b.s[1];
            ct=b.ct;act=b.act;
            rb=b.rb;
            _key=std::move(b._key);
        }
        int f,s[2],ct,act;
        bool rb;
        key _key;
    };
    node*tr;
    int used,size;
    int(*__cmp__)(const key&,const key&);
    bool __id__(int x){return tr[tr[x].f].s[1]==x;}
    void __update__(int x){tr[x].act=tr[tr[x].s[0]].act+tr[tr[x].s[1]].act+tr[x].ct;}
    void __rotate__(int x){
        bool to=__id__(x);
        int y=tr[x].f,z=tr[y].f,w=tr[x].s[to^1];
        tr[x].f=z;tr[z].s[__id__(y)]=x;
        tr[w].f=y;tr[y].s[to]=w;
        tr[y].f=x;tr[x].s[to^1]=y;
        __update__(y);__update__(x);
    }
    bool __resize__(int tarsize){
        node*newtr=new(std::nothrow)node[tarsize];
        if(newtr==NULL)return 1;
        for(int i=0;i<used;i++)newtr[i]=std::move(tr[i]);
        size=tarsize;
        delete[] tr;tr=newtr;
        return 0;
    }
    template<class __key>
    bool __insert__(__key&&v){
        int x=tr[0].s[1],y;
        if(x==0){
            x=used++;
            tr[0].s[0]=tr[0].s[1]=x;
            tr[x].f=tr[x].s[0]=tr[x].s[1]=tr[x].rb=0;
            tr[x].act=tr[x].ct=1;
            tr[x]._key=std::forward<__key>(v);
            return 0;
        }
        if(used>=size&&__resize__(size<<(size<100000?2:1)))return 1;
        while(1){
            tr[x].act++;
            int to=__cmp__(v,tr[x]._key);
            if(to==2){tr[x].ct++;return 0;}
            if(tr[x].s[to]==0){
                y=x;x=used++;
                tr[y].s[to]=x;tr[x].f=y;
                tr[x].s[0]=tr[x].s[1]=0;
                tr[x].act=tr[x].ct=tr[x].rb=1;
                tr[x]._key=std::forward<__key>(v);
                break;
            }
            x=tr[x].s[to];
        }
        while(y&&tr[x].rb&&tr[y].rb){
            int to=__id__(y),z=tr[y].f,w=tr[z].s[to^1];
            if(tr[w].rb==0){
                if(to!=__id__(x))__rotate__(x),y=x;
                tr[y].rb=0;tr[z].rb=1;
                __rotate__(y);
                break;
            }
            tr[y].rb=tr[w].rb=0;tr[z].rb=1;
            x=z;y=tr[z].f;
        }
        tr[tr[0].s[1]].rb=0;
        tr[0].s[0]=tr[0].s[1];
        return 0;
    }
    bool __delete__(const key&v){
        int x=tr[0].s[1];
        while(1){
            if(x==0)return 1;
            int to=__cmp__(v,tr[x]._key);
            if(to==2)break;
            x=tr[x].s[to];
        }
        for(int y=x;y;y=tr[y].f)tr[y].act--;
        if(tr[x].ct>1){tr[x].ct--;return 0;}
        if(tr[x].s[0]&&tr[x].s[1]){
            int y=x;x=tr[x].s[1];
            while(tr[x].s[0])x=tr[x].s[0];
            tr[y].ct=tr[x].ct;
            tr[y]._key=std::move(tr[x]._key);
            for(int ct=tr[x].ct,tempy=tr[x].f;tempy!=y;tempy=tr[tempy].f)tr[tempy].ct-=ct;
        }
        int y=tr[x].f,w=tr[x].s[0]|tr[x].s[1],l,r;
        bool to=__id__(x);
        tr[y].s[to]=w;tr[w].f=y;
        tr[w].rb=0;
        if(w||tr[x].rb)y=0;
        if(x!=--used){
            tr[x]=std::move(tr[used]);
            tr[tr[x].f].s[__id__(used)]=tr[tr[x].s[0]].f=tr[tr[x].s[1]].f=x;
        }
        else tr[x]=node();
        while(y){
            w=tr[y].s[to^1];
            if(tr[w].rb){tr[y].rb=1;tr[w].rb=0;__rotate__(w);w=tr[y].s[to^1];}
            l=tr[w].s[to];r=tr[w].s[to^1];
            if(tr[r].rb){tr[w].rb=tr[y].rb;tr[y].rb=tr[r].rb=0;__rotate__(w);break;}
            if(tr[l].rb){tr[l].rb=tr[y].rb;tr[y].rb=0;__rotate__(l);__rotate__(l);break;}
            tr[w].rb=1;
            if(tr[y].rb){tr[y].rb=0;break;}
            x=y,y=tr[x].f,to=__id__(x);
        }
        if(used<<3<=size)__resize__(size>>1);
        tr[0].s[0]=tr[0].s[1];
        return 0;
    }
    bool __reinit__(bool do_alloc){
        node*newtr=NULL;
        if(do_alloc){
            newtr=new(std::nothrow)node[4];
            if(newtr==NULL)return 1;
            newtr->act=newtr->ct=newtr->rb=newtr->f=newtr->s[0]=newtr->s[1]=0;
        }
        delete[] tr;tr=newtr;
        used=1;size=4;
        return 0;
    }
    Iterator __kth__(int k)const{
        int x=tr[0].s[1],ls;
        while(x){
            ls=tr[x].s[0];
            if(k<tr[ls].act){x=ls;continue;}
            k-=tr[ls].act;
            if(k<tr[x].ct)break;
            k-=tr[x].ct;x=tr[x].s[1];
        }
        return Iterator(*this,x);
    }
    int __find__(const key&v)const{
        int x=tr[0].s[1],res=0,to;
        while(x){
            to=__cmp__(v,tr[x]._key);
            if(to==0){x=tr[x].s[0];continue;}
            res+=tr[tr[x].s[0]].act;
            if(to==2)return res;
            res+=tr[x].ct;
            x=tr[x].s[1];
        }
        return -1;
    }
    Iterator __loc__(const key&v)const{
        int x=tr[0].s[1],to;
        while(x){
            to=__cmp__(v,tr[x]._key);
            if(to==2)break;
            x=tr[x].s[to];
        }
        return Iterator(*this,x);
    }
    Iterator __lowerbound__(const key&v)const{
        int x=tr[0].s[1],maxx=0;
        while(x){
            if(__cmp__(tr[x]._key,v)==0){maxx=x;x=tr[x].s[1];break;}
            tr[x].s[0];
        }
        while(x){
            if(__cmp__(tr[x]._key,v)==0){
                if(__cmp__(tr[maxx]._key,tr[x]._key)==0)maxx=x;
                x=tr[x].s[1];
            }
            else x=tr[x].s[0];
        }
        return Iterator(*this,maxx);
    }
    Iterator __upperbound__(const key&v)const{
        int x=tr[0].s[1],minx=0;
        while(x){
            if(__cmp__(v,tr[x]._key)==0){minx=x;x=tr[x].s[0];break;}
            x=tr[x].s[1];
        }
        while(x){
            if(__cmp__(v,tr[x]._key)==0){
                if(__cmp__(tr[x]._key,tr[minx]._key)==0)minx=x;
                x=tr[x].s[0];
            }
            else x=tr[x].s[1];
        }
        return Iterator(*this,minx);
    }
    Iterator __begin__()const{
        if(tr==NULL)return Iterator();
        int x=tr[0].s[1];
        while(tr[x].s[0])x=tr[x].s[0];
        return Iterator(*this,x);
    }
    Iterator __end__()const{
        return Iterator(*this,0);
    }
    static int __defaultcmp__(const key&a,const key&b){
        return a<b?0:b<a?1:2;
    }
    static void __defaultprint__(const key&v){std::cout<<v;}
};
}
using namespace _Alter;
int main(){
    int a=clock();
    rbtree<int>t(1);
    int b=clock();
    for(int i=0;i<1000000;i++)t.insert(10);
    int c=clock();
    // for(int i=0;i<1000000;i++)t.remove(i);
    for(int i=0;i<1000000;i++)t.begin().getcount();
    int d=clock();
    std::cout<<(b-a)/1000.0<<' '<<(c-b)/1000.0<<' '<<(d-c)/1000.0<<std::endl;
}