#include<stdlib.h>
#include<stdbool.h>
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
    void(*pnode)(node*t);
};
void _init_(Chris*t,ll(*cmp)(void*a,void*b),void(*pnode)(node*t)){
    t->cmp=cmp;
    t->pnode=pnode;
}
bool id(node*x){return x->f->s[1]==x;}
void update(node*x){
    x->act=x->ct;
    if(x->s[0]!=NULL)x->act+=x->s[0]->act;
    if(x->s[1]!=NULL)x->act+=x->s[1]->act;
}
void rotate(node*x){
    bool j=id(x);node*y=x->f,*z=y->f,*w=x->s[j^1];
    x->f=z;if(z!=NULL)z->s[id(y)]=x;
    if(w!=NULL)w->f=y;y->s[j]=w;
    y->f=x;x->s[j^1]=y;
    update(y);update(x);
}
void insert(Chris*t,void*v){
    if(t->rt==NULL){
        t->rt=(node*)calloc(1,sizeof(node));
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
            node*new=(node*)calloc(1,sizeof(node));
            new->f=x,x->s[tar]=new;
            new->key=v;
            new->rb=new->act=new->ct=1;
            x=new;
            break;
        }
        x=x->s[tar];
    }
    for(node*y=x->f;y!=NULL&&x->rb&&y->rb;){
        ll j=id(y);node*z=y->f,*w=z->s[j^1];
        if(w==NULL||!w->rb){
            if(j!=id(x))rotate(x),y=x;
            y->rb=0,z->rb=1;
            rotate(y);
            if(y->f==NULL)t->rt=y;
            return;
        }
        y->rb=w->rb=0,z->rb=1;
        x=z,y=z->f;
    }
    t->rt->rb=0;
}
void f_all(node*x){
    if(x==NULL)return;
    f_all(x->s[0]);
    f_all(x->s[1]);
    free(x);
}
void dfs(Chris*t,node*x,ll depth){
    if(x==NULL)return;
    dfs(t,x->s[0],depth+1);
    t->pnode(x);printf("%d\n",depth);
    dfs(t,x->s[1],depth+1);
}