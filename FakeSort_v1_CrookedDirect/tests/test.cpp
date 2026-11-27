#define FAKE_SORT_LIBRARY
#include "../src/FakeSort.cpp"
#include <random>
#include <limits>
int main(){
 size_t exhaustive=0,parallel=0,rejected=0;
 auto check=[](V a,int m,int th,int cut){V ref=a;std::sort(ref.begin(),ref.end());fake_sort(a,m,th,cut);if(a!=ref)throw std::runtime_error("sort mismatch");};
 for(int n=0,p=1;n<=8;n++,p*=3)for(int k=0;k<p;k++){V a(n);int x=k;for(auto& v:a){v=x%3-1;x/=3;}for(int m=0;m<6;m++){check(a,m,1,16);exhaustive++;}}
 std::mt19937 rng(42);
 for(int n:{9,127,1024,4096})for(int kind=0;kind<5;kind++){V a(n);for(int i=0;i<n;i++)a[i]=kind==0?double(rng()%100000)/7:kind==1?double(i):kind==2?-double(i):kind==3?double(rng()%8):7;for(int m:{3,4})for(int th:{1,2,4})for(int cut:{2,16,2048}){check(a,m,th,cut);parallel++;}}
 for(int m=0;m<6;m++)for(double x:{std::numeric_limits<double>::infinity(),-std::numeric_limits<double>::infinity(),std::numeric_limits<double>::quiet_NaN()}){V a{x};try{fake_sort(a,m);throw std::runtime_error("accepted invalid value");}catch(const std::invalid_argument&){rejected++;}}
 for(int k=0;k<3;k++){V a{2,1};try{fake_sort(a,k==0?6:0,k==1?0:1,k==2?1:16);throw std::runtime_error("accepted invalid option");}catch(const std::invalid_argument&){rejected++;}}
 std::cout<<"PASS exhaustive "<<exhaustive<<"\nPASS parallel configurations "<<parallel<<"\nPASS rejection "<<rejected<<"\n";
}
