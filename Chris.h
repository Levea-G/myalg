#include<stdlib.h>
#include<stdbool.h>
#include<stdarg.h>
typedef struct Chris Chris;
typedef struct node node;
typedef int ll;
struct node{
    bool rb;
    ll ct,act;
    void*key;
    node*f,*s[2];
};
struct Chris{
    node*rt;
    ll(*cmp)(void*a,void*b);
    void(*print)(void*key);
};
void __init__(Chris*t,ll(*cmp)(void*a,void*b),void(*print)(void*key)){
    if(t->cmp==NULL)t->cmp=cmp;
    else if(cmp!=NULL)printf("can't change way of comparing!\n");
    t->print=print;
}
bool __id__(node*x){return x->f->s[1]==x;}
void __update__(node*x){
    x->act=x->ct;
    if(x->s[0]!=NULL)x->act+=x->s[0]->act;
    if(x->s[1]!=NULL)x->act+=x->s[1]->act;
}
void __rotate__(node*x){
    bool j=__id__(x);node*y=x->f,*z=y->f,*w=x->s[j^1];
    x->f=z;if(z!=NULL)z->s[__id__(y)]=x;
    if(w!=NULL)w->f=y;y->s[j]=w;
    y->f=x;x->s[j^1]=y;
    __update__(y);__update__(x);
}
void insert(Chris*t,void*v){
    if(t->cmp==NULL||v==NULL)return;
    if(t->rt==NULL){
        t->rt=(node*)calloc(1,28);
        t->rt->key=v;
        t->rt->act=t->rt->ct=1;
        return;
    }
    node*x=t->rt;
    while(1){
        x->act++;
        ll tar=t->cmp(v,x->key);
        if(tar==2){x->ct++;return;}
        if(x->s[tar]==NULL){
            node*new=(node*)calloc(1,28);
            new->f=x,x->s[tar]=new;
            new->key=v;
            new->rb=new->act=new->ct=1;
            x=new;
            break;
        }
        x=x->s[tar];
    }
    for(node*y=x->f;y!=NULL&&x->rb&&y->rb;){
        ll j=__id__(y);node*z=y->f,*w=z->s[j^1];
        if(w==NULL||!w->rb){
            if(j!=__id__(x))__rotate__(x),y=x;
            y->rb=0,z->rb=1;
            __rotate__(y);if(y->f==NULL)t->rt=y;
            return;
        }
        y->rb=w->rb=0,z->rb=1;
        x=z,y=z->f;
    }
    t->rt->rb=0;
}
void delet(Chris*t,void*v){
    if(t->cmp==NULL||v==NULL)return;
    node*x=t->rt;
    for(ll tar;;){
        if(x==NULL)return;
        tar=t->cmp(v,x->key);
        if(tar==2)break;
        x=x->s[tar];
    }
    for(node*temp=x;temp!=NULL;temp=temp->f)temp->act--;
    if(x->ct>1){x->ct--;return;}
    if(x->s[0]!=NULL&&x->s[1]!=NULL){
        node*y=x;x=x->s[1];
        while(x->s[0]!=NULL)x=x->s[0];
        y->key=x->key,y->ct=x->ct;
        for(node*temp=x->f;temp!=y;temp=temp->f)temp->act-=x->ct;
    }
    node*y=x->f,*temp=x->s[0]==NULL?x->s[1]:x->s[0];ll j;
    if(y!=NULL)j=__id__(x),y->s[j]=temp;
    else t->rt=temp;
    if(temp!=NULL)temp->f=y,temp->rb=0,y=NULL;
    else if(x->rb)y=NULL;
    free(x);
    if(y==NULL)return;
    while(1){
        node*w=y->s[j^1];
        if(w->rb){
            y->rb=1,w->rb=0;
            __rotate__(w);if(w->f==NULL)t->rt=w;
            w=y->s[j^1];
        }
        node*l=w->s[j],*r=w->s[j^1];
        if(r!=NULL&&r->rb){
            w->rb=y->rb,y->rb=r->rb=0;
            __rotate__(w);if(w->f==NULL)t->rt=w;
            return;
        }
        if(l!=NULL&&l->rb){
            l->rb=y->rb,y->rb=0;
            __rotate__(l),__rotate__(l);if(l->f==NULL)t->rt=l;
            return;
        }
        w->rb=1;
        if(y->rb){y->rb=0;return;}
        x=y,y=x->f;if(y==NULL)return;
        j=__id__(x);
    }
}
ll rank(Chris*t,void*v){
    if(t->cmp==NULL)return -1;
    node*x=t->rt;ll otc=0;
    while(x!=NULL){
        ll tar=t->cmp(v,x->key);
        if(tar==2)return otc+1;
        otc+=tar*(x->ct);
        x=x->s[tar];
    }
    return -1;
}
void*pre(Chris*t,void*v){
    if(t->cmp==NULL||t->rt==NULL)return NULL;
    node*x=t->rt;void*otc=NULL;
    while(x!=NULL){
        if(!t->cmp(x->key,v)){
            if(otc==NULL)otc=x->key;
            else if(!t->cmp(otc,x->key))otc=x->key;
            x=x->s[1];
        }
        else x=x->s[0];
    }
    return otc;
}
void*next(Chris*t,void*v){
    if(t->cmp==NULL||t->rt==NULL)return NULL;
    node*x=t->rt;void*otc=NULL;
    while(x!=NULL){
        if(!t->cmp(v,x->key)){
            if(otc==NULL)otc=x->key;
            else if(!t->cmp(x->key,otc))otc=x->key;
            x=x->s[0];
        }
        else x=x->s[1];
    }
    return otc;
}
bool has(Chris*t,void*v){
    if(t->cmp==NULL)return 0;
    node*x=t->rt;
    while(x!=NULL){
        ll tar=t->cmp(v,x->key);
        if(tar==2)return 1;
        x=x->s[tar];
    }
    return 0;
}
void __FREE__(node*x){
    if(x==NULL)return;
    __FREE__(x->s[0]);
    __FREE__(x->s[1]);
    free(x->key);
    free(x);
}
void clear(Chris*t){
    __FREE__(t->rt);
    free(t);
}
void view(Chris*t,char*sep,...){
    node*x=t->rt;
    if(x==NULL)return;
    va_list args;
    va_start(args,sep);
    void(*print)(void*key)=va_arg(args,void*);
    va_end(args);
    if(print==NULL)
        if(t->print==NULL){printf("print method undefined\n");return;}
        else print=t->print;
    l1:while(x->s[0]!=NULL)x=x->s[0];
    print(x->key);printf("%s",sep);
    l2:if(x->s[1]!=NULL){x=x->s[1];goto l1;}
    while(x->f!=NULL){
        ll j=__id__(x);x=x->f;
        if(!j){print(x->key);printf("%s",sep);goto l2;}
    }
}