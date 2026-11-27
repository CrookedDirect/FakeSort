#include <cmath>
// M08: C++17 single-thread port; logical element counters, not hardware counters.
#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <vector>
#include <stdexcept>
#include <string>
using namespace std; using V=vector<double>;
struct Stats{uint64_t rd=0,wr=0,co=0,parts=0,adapt=0,rooms=0;};
struct Room{int l,r;bool flag,parity;};
template<bool S> double read(const V& a,int i,Stats& t){if constexpr(S)++t.rd;return a[i];}
template<bool S> void write(V& a,int i,double v,Stats& t){if constexpr(S)++t.wr;a[i]=v;}
template<bool S> bool lt(double a,double b,Stats& t){if constexpr(S)++t.co;return a<b;}
template<bool S> void small(V& a,const V& src,int l,int n,Stats& t){switch(n){
case 2: {double v0=read<S>(src,l+0,t);double v1=read<S>(src,l+1,t);if(lt<S>(v1,v0,t))swap(v0,v1);write<S>(a,l+0,v0,t);write<S>(a,l+1,v1,t);break;}
case 3: {double v0=read<S>(src,l+0,t);double v1=read<S>(src,l+1,t);double v2=read<S>(src,l+2,t);if(lt<S>(v1,v0,t))swap(v0,v1);if(lt<S>(v2,v1,t))swap(v1,v2);if(lt<S>(v1,v0,t))swap(v0,v1);write<S>(a,l+0,v0,t);write<S>(a,l+1,v1,t);write<S>(a,l+2,v2,t);break;}
case 4: {double v0=read<S>(src,l+0,t);double v1=read<S>(src,l+1,t);double v2=read<S>(src,l+2,t);double v3=read<S>(src,l+3,t);if(lt<S>(v1,v0,t))swap(v0,v1);if(lt<S>(v3,v2,t))swap(v2,v3);if(lt<S>(v2,v0,t))swap(v0,v2);if(lt<S>(v3,v1,t))swap(v1,v3);if(lt<S>(v2,v1,t))swap(v1,v2);write<S>(a,l+0,v0,t);write<S>(a,l+1,v1,t);write<S>(a,l+2,v2,t);write<S>(a,l+3,v3,t);break;}
case 5: {double v0=read<S>(src,l+0,t);double v1=read<S>(src,l+1,t);double v2=read<S>(src,l+2,t);double v3=read<S>(src,l+3,t);double v4=read<S>(src,l+4,t);if(lt<S>(v3,v0,t))swap(v0,v3);if(lt<S>(v4,v1,t))swap(v1,v4);if(lt<S>(v2,v0,t))swap(v0,v2);if(lt<S>(v3,v1,t))swap(v1,v3);if(lt<S>(v1,v0,t))swap(v0,v1);if(lt<S>(v4,v2,t))swap(v2,v4);if(lt<S>(v2,v1,t))swap(v1,v2);if(lt<S>(v4,v3,t))swap(v3,v4);if(lt<S>(v3,v2,t))swap(v2,v3);write<S>(a,l+0,v0,t);write<S>(a,l+1,v1,t);write<S>(a,l+2,v2,t);write<S>(a,l+3,v3,t);write<S>(a,l+4,v4,t);break;}
case 6: {double v0=read<S>(src,l+0,t);double v1=read<S>(src,l+1,t);double v2=read<S>(src,l+2,t);double v3=read<S>(src,l+3,t);double v4=read<S>(src,l+4,t);double v5=read<S>(src,l+5,t);if(lt<S>(v5,v0,t))swap(v0,v5);if(lt<S>(v3,v1,t))swap(v1,v3);if(lt<S>(v4,v2,t))swap(v2,v4);if(lt<S>(v2,v1,t))swap(v1,v2);if(lt<S>(v4,v3,t))swap(v3,v4);if(lt<S>(v3,v0,t))swap(v0,v3);if(lt<S>(v5,v2,t))swap(v2,v5);if(lt<S>(v1,v0,t))swap(v0,v1);if(lt<S>(v3,v2,t))swap(v2,v3);if(lt<S>(v5,v4,t))swap(v4,v5);if(lt<S>(v2,v1,t))swap(v1,v2);if(lt<S>(v4,v3,t))swap(v3,v4);write<S>(a,l+0,v0,t);write<S>(a,l+1,v1,t);write<S>(a,l+2,v2,t);write<S>(a,l+3,v3,t);write<S>(a,l+4,v4,t);write<S>(a,l+5,v5,t);break;}
case 7: {double v0=read<S>(src,l+0,t);double v1=read<S>(src,l+1,t);double v2=read<S>(src,l+2,t);double v3=read<S>(src,l+3,t);double v4=read<S>(src,l+4,t);double v5=read<S>(src,l+5,t);double v6=read<S>(src,l+6,t);if(lt<S>(v6,v0,t))swap(v0,v6);if(lt<S>(v3,v2,t))swap(v2,v3);if(lt<S>(v5,v4,t))swap(v4,v5);if(lt<S>(v2,v0,t))swap(v0,v2);if(lt<S>(v4,v1,t))swap(v1,v4);if(lt<S>(v6,v3,t))swap(v3,v6);if(lt<S>(v1,v0,t))swap(v0,v1);if(lt<S>(v5,v2,t))swap(v2,v5);if(lt<S>(v4,v3,t))swap(v3,v4);if(lt<S>(v2,v1,t))swap(v1,v2);if(lt<S>(v6,v4,t))swap(v4,v6);if(lt<S>(v3,v2,t))swap(v2,v3);if(lt<S>(v5,v4,t))swap(v4,v5);if(lt<S>(v2,v1,t))swap(v1,v2);if(lt<S>(v4,v3,t))swap(v3,v4);if(lt<S>(v6,v5,t))swap(v5,v6);write<S>(a,l+0,v0,t);write<S>(a,l+1,v1,t);write<S>(a,l+2,v2,t);write<S>(a,l+3,v3,t);write<S>(a,l+4,v4,t);write<S>(a,l+5,v5,t);write<S>(a,l+6,v6,t);break;}
case 8: {double v0=read<S>(src,l+0,t);double v1=read<S>(src,l+1,t);double v2=read<S>(src,l+2,t);double v3=read<S>(src,l+3,t);double v4=read<S>(src,l+4,t);double v5=read<S>(src,l+5,t);double v6=read<S>(src,l+6,t);double v7=read<S>(src,l+7,t);if(lt<S>(v2,v0,t))swap(v0,v2);if(lt<S>(v3,v1,t))swap(v1,v3);if(lt<S>(v6,v4,t))swap(v4,v6);if(lt<S>(v7,v5,t))swap(v5,v7);if(lt<S>(v4,v0,t))swap(v0,v4);if(lt<S>(v5,v1,t))swap(v1,v5);if(lt<S>(v6,v2,t))swap(v2,v6);if(lt<S>(v7,v3,t))swap(v3,v7);if(lt<S>(v1,v0,t))swap(v0,v1);if(lt<S>(v3,v2,t))swap(v2,v3);if(lt<S>(v5,v4,t))swap(v4,v5);if(lt<S>(v7,v6,t))swap(v6,v7);if(lt<S>(v4,v2,t))swap(v2,v4);if(lt<S>(v5,v3,t))swap(v3,v5);if(lt<S>(v4,v1,t))swap(v1,v4);if(lt<S>(v6,v3,t))swap(v3,v6);if(lt<S>(v2,v1,t))swap(v1,v2);if(lt<S>(v4,v3,t))swap(v3,v4);if(lt<S>(v6,v5,t))swap(v5,v6);write<S>(a,l+0,v0,t);write<S>(a,l+1,v1,t);write<S>(a,l+2,v2,t);write<S>(a,l+3,v3,t);write<S>(a,l+4,v4,t);write<S>(a,l+5,v5,t);write<S>(a,l+6,v6,t);write<S>(a,l+7,v7,t);break;}
}}
template<int M,bool S,bool DFS=false> void pws(V& a,Stats& t,V* scratch=nullptr, array<vector<Room>,2>* reuse=nullptr){int n=a.size();if(n<2)return;if constexpr(false){if(n<=8){small<S>(a,a,0,n,t);return;}} V own;if(!scratch)own.resize(n);V& b=scratch?*scratch:own;vector<Room> lr,ln;auto& rooms=reuse?(*reuse)[0]:lr;auto& next=reuse?(*reuse)[1]:ln;rooms.clear();next.clear();rooms.push_back({0,n-1,false,false});while(!rooms.empty()){next.clear();size_t count=DFS?1:rooms.size();for(size_t j=0;j<count;j++){Room q=DFS?rooms.back():rooms[j];if constexpr(DFS)rooms.pop_back();auto& src=q.parity?b:a;auto& dst=q.parity?a:b;int l=q.l,r=q.r,pi=l+(r-l)/(M>=1&&q.flag?4:2),w=l,z=r;double p=read<S>(src,pi,t);if constexpr(S){++t.parts;t.adapt+=q.flag;}auto low=[&](int i){double v=read<S>(src,i,t);if(lt<S>(v,p,t))write<S>(dst,w++,v,t);else if(lt<S>(p,v,t))write<S>(dst,z--,v,t);};if constexpr(M==4){for(int i=l;i<=pi;i++)low(i);for(int i=r;i>pi;i--){double v=read<S>(src,i,t);if(lt<S>(p,v,t))write<S>(dst,z--,v,t);else if(lt<S>(v,p,t))write<S>(dst,w++,v,t);}}else for(int i=l;i<=r;i++)low(i);if(!(M>=3&&!q.parity&&w==l&&z==r))for(int i=w;i<=z;i++)write<S>(a,i,p,t);auto child=[&](int x,int y){int k=y-x+1;if(k<=0)return;if(k==1){if(!q.parity)write<S>(a,x,read<S>(dst,x,t),t);return;}if constexpr(false){if(k<=8){small<S>(a,dst,x,k,t);return;}}(DFS?rooms:next).push_back({x,y,M>=1&&8*k>7*(r-l+1),!q.parity});};child(l,w-1);child(z+1,r);}if constexpr(!DFS){if(reuse)rooms.swap(next);else rooms=move(next);}}}
template<int M,bool S> void qs(V& a,Stats& t){int n=a.size();if(n<2)return;if constexpr(false){if(n<=8){small<S>(a,a,0,n,t);return;}}vector<Room> rooms{{0,n-1,false,false}};while(!rooms.empty()){vector<Room> next;for(auto q:rooms){int l=q.l,r=q.r,i=l,w=l,z=r;double p=read<S>(a,l+(r-l)/(M>=1&&q.flag?4:2),t);if constexpr(S){++t.parts;t.adapt+=q.flag;}while(i<=z){double v=read<S>(a,i,t);if(lt<S>(v,p,t)){write<S>(a,i,read<S>(a,w,t),t);write<S>(a,w,v,t);++w;++i;}else if(lt<S>(p,v,t)){write<S>(a,i,read<S>(a,z,t),t);write<S>(a,z,v,t);--z;}else ++i;}auto child=[&](int x,int y){int k=y-x+1;if(k<2)return;if constexpr(false){if(k<=8){small<S>(a,a,x,k,t);return;}}next.push_back({x,y,M>=1&&8*k>7*(r-l+1),false});};child(l,w-1);child(z+1,r);}rooms=move(next);}}
template<bool S> void run(int q,V& a,Stats& t,V* b=nullptr){switch(q){case 0:pws<0,S>(a,t,b);break;case 1:pws<1,S>(a,t,b);break;case 2:pws<2,S>(a,t,b);break;case 3:pws<3,S>(a,t,b);break;case 4:pws<4,S>(a,t,b);break;case 5:qs<0,S>(a,t);break;case 6:qs<1,S>(a,t);break;case 7:qs<2,S>(a,t);break;case 8:sort(a.begin(),a.end());break;case 9:pws<4,S,true>(a,t,b);break;}}
V make(int n,int k,uint32_t seed){auto rnd=[&](int m){seed=seed*1664525u+1013904223u;return int(uint64_t(seed)*m>>32);};V a(n);for(int i=0;i<n;i++){switch(k){case 0:a[i]=rnd(n*4);break;case 1:case 3:a[i]=i;break;case 2:a[i]=n-i;break;case 4:a[i]=7;break;case 5:a[i]=rnd(32);break;case 6:a[i]=min(i,n-1-i);break;case 7:a[i]=i%97;break;case 8:a[i]=abs(i-n/2);break;}}if(k==3&&n>0)for(int i=0;i<max(1.0,n/100.0);i++){int x=rnd(n),y=rnd(n);swap(a[x],a[y]);}return a;}
double med(vector<double> x){sort(x.begin(),x.end());return (x[(x.size()-1)/2]+x[x.size()/2])/2;}


#include <omp.h>
template<class Emit> void partition_room(V& a,V& b,Room q,Emit emit){Stats t;
auto& src=q.parity?b:a;auto& dst=q.parity?a:b;int l=q.l,r=q.r,pi=l+(r-l)/(true&&q.flag?4:2),w=l,z=r;double p=read<false>(src,pi,t);if constexpr(false){++t.parts;t.adapt+=q.flag;}auto low=[&](int i){double v=read<false>(src,i,t);if(lt<false>(v,p,t))write<false>(dst,w++,v,t);else if(lt<false>(p,v,t))write<false>(dst,z--,v,t);};if constexpr(true){for(int i=l;i<=pi;i++)low(i);for(int i=r;i>pi;i--){double v=read<false>(src,i,t);if(lt<false>(p,v,t))write<false>(dst,z--,v,t);else if(lt<false>(v,p,t))write<false>(dst,w++,v,t);}}else for(int i=l;i<=r;i++)low(i);if(!(true&&!q.parity&&w==l&&z==r))for(int i=w;i<=z;i++)write<false>(a,i,p,t);auto child=[&](int x,int y){int k=y-x+1;if(k<=0)return;if(k==1){if(!q.parity)write<false>(a,x,read<false>(dst,x,t),t);return;}if constexpr(false){if(k<=8){small<false>(a,dst,x,k,t);return;}}emit({x,y,true&&8*k>7*(r-l+1),!q.parity});};child(l,w-1);child(z+1,r);
}
void descend(V& a,V& b,Room q){partition_room(a,b,q,[&](Room c){descend(a,b,c);});}
void task_room(V& a,V& b,Room q,int cutoff){partition_room(a,b,q,[&](Room c){if(c.r-c.l+1<cutoff){descend(a,b,c);}else{
V* ap=&a; V* bp=&b;
#pragma omp task firstprivate(ap,bp,c,cutoff)
{task_room(*ap,*bp,c,cutoff);}
}});}
void async_sort(V& a,V& b,int threads,int cutoff){if(a.size()<2)return;Stats t;if(false){small<false>(a,a,0,a.size(),t);return;}
#pragma omp parallel num_threads(threads)
{
#pragma omp single
{
#pragma omp taskgroup
{task_room(a,b,{0,int(a.size())-1,false,false},cutoff);}
}
}}
void wave_sort(V& a,V& b,int threads){if(a.size()<2)return;Stats t;if(false){small<false>(a,a,0,a.size(),t);return;}vector<Room> rooms{{0,int(a.size())-1,false,false}},merged;vector<vector<Room>> local(threads);bool done=false;
#pragma omp parallel num_threads(threads)
{auto& next=local[omp_get_thread_num()];while(true){
#pragma omp single
{done=rooms.empty();}
if(done)break;next.clear();
#pragma omp for schedule(static)
for(size_t j=0;j<rooms.size();j++)partition_room(a,b,rooms[j],[&](Room c){next.push_back(c);});
#pragma omp single
{merged.clear();for(auto& v:local)merged.insert(merged.end(),v.begin(),v.end());rooms.swap(merged);}
}}}
void qt(V& a,int l,int r,bool f,int cut){if(l>=r)return;Stats t;int n=r-l+1;if(false){small<false>(a,a,l,n,t);return;}double p=a[l+(r-l)/(f?4:2)];int i=l,w=l,z=r;while(i<=z){double v=a[i];if(v<p){swap(a[i++],a[w++]);}else if(p<v){swap(a[i],a[z--]);}else ++i;}auto child=[&](int x,int y){int k=y-x+1;if(k<2)return;bool g=8*k>7*n;V* ap=&a;if(k<cut)qt(a,x,y,g,cut);else{
#pragma omp task firstprivate(ap,x,y,g,cut)
{qt(*ap,x,y,g,cut);}}};child(l,w-1);child(z+1,r);}
void qp(V& a,int th){
#pragma omp parallel num_threads(th)
{
#pragma omp single
{
#pragma omp taskgroup
{qt(a,0,int(a.size())-1,false,2048);}
}}}


void mergepure(V& a,V& b,int l,int r){if(r-l<2)return;int m=l+(r-l)/2;mergepure(a,b,l,m);mergepure(a,b,m,r);int i=l,j=m,k=l;while(i<m&&j<r)b[k++]=a[i]<=a[j]?a[i++]:a[j++];while(i<m)b[k++]=a[i++];while(j<r)b[k++]=a[j++];copy(b.begin()+l,b.begin()+r,a.begin()+l);}
void hoare(V& a,int l,int r){while(l<r){double p=a[l+(r-l)/2];int i=l-1,j=r+1;while(true){do{++i;}while(a[i]<p);do{--j;}while(a[j]>p);if(i>=j)break;swap(a[i],a[j]);}if(j-l<r-j-1){hoare(a,l,j);l=j+1;}else{hoare(a,j+1,r);r=j;}}}
int main(){omp_set_dynamic(0);cout<<fixed<<setprecision(6);long checks=0;for(int n:{256,1000,10000,100000})for(int k=0;k<9;k++)for(unsigned seed:{12345u,54321u,98765u}){V base=make(n,k,seed),ref=base,b(n);sort(ref.begin(),ref.end());array<vector<Room>,2> reuse;int groups=n<=1000?11:9;vector<vector<double>> times(groups);auto measure=[&](int q){if(q==0&&n>10000&&(k==6||k==8))return -1.0;V a=base;Stats t;auto start=chrono::steady_clock::now();switch(q){case 0:pws<0,false>(a,t,&b,&reuse);break;case 1:pws<4,false>(a,t,&b,&reuse);break;case 2:qs<1,false>(a,t);break;case 3:hoare(a,0,n-1);break;case 4:mergepure(a,b,0,n);break;case 5:make_heap(a.begin(),a.end());sort_heap(a.begin(),a.end());break;case 6:async_sort(a,b,4,2048);break;case 7:qp(a,4);break;case 8:async_sort(a,b,1,2048);break;case 9:for(int i=1;i<n;i++){double v=a[i];int j=i;while(j>0&&a[j-1]>v){a[j]=a[j-1];--j;}a[j]=v;}break;case 10:for(int e=n-1;e>0;--e){bool c=false;for(int j=0;j<e;j++)if(a[j]>a[j+1]){swap(a[j],a[j+1]);c=true;}if(!c)break;}break;}auto end=chrono::steady_clock::now();if(a!=ref)throw runtime_error("incorrect");++checks;return chrono::duration<double,milli>(end-start).count();};for(int q=0;q<groups;q++)measure(q);for(int j=0;j<5;j++)for(int i=0;i<groups;i++){int q=(i+j*3)%groups;times[q].push_back(measure(q));}cout<<n<<' '<<k<<' '<<seed;for(auto &v:times)cout<<' '<<med(v);cout<<endl;}cout<<"PASS "<<checks<<endl;}
