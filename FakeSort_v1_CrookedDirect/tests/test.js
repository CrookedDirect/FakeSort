'use strict';
const assert=require('node:assert/strict');
const {fakeSort}=require('../src/FakeSort.js');
let counts={};let seed=42;const rnd=()=>((seed=Math.imul(seed,1664525)+1013904223>>>0)/4294967296);
function check(values,mode,T,scratchType,group){const a=T.from(values),ref=Array.from(a).sort((x,y)=>x-y);const scratch=scratchType?new scratchType(a.length+3):null;assert.equal(fakeSort(a,{mode,scratch}),a);assert.deepEqual(Array.from(a).map(x=>x===0?0:x),ref.map(x=>x===0?0:x));counts[group]=(counts[group]||0)+1;}
for(const mode of ['original','pure','advanced','optimized']){
 for(let n=0;n<=128;n++)for(let k=0;k<5;k++)for(const T of [Array,Float64Array])check(Array.from({length:n},(_,i)=>[ (i*73+19)%31,i,-i,7,Math.abs(i-Math.floor(n/2))][k]),mode,T,Float64Array,'historical-style');
 for(let n=0;n<=8;n++)for(let mask=0;mask<3**n;mask++){let x=mask;const a=Array.from({length:n},()=>{const v=x%3-1;x=Math.floor(x/3);return v;});check(a,mode,Array,null,'exhaustive ternary');}
 for(const T of [Array,Float64Array])for(const S of [null,Array,Float64Array])for(const n of [0,1,2,8,9,31,128,1024,10000])check(Array.from({length:n},()=> (rnd()-.5)*1e6),mode,T,S,'fractional scratch matrix');
 for(const T of [Array,Float64Array])check([Number.MAX_VALUE,-Number.MAX_VALUE,Number.MIN_VALUE,-Number.MIN_VALUE,0,-0,1.5,-2.5,1e-200],mode,T,null,'finite extremes');
}
const bad=[()=>fakeSort([1,2],{mode:'bad'}),()=>fakeSort([NaN]),()=>fakeSort([Infinity]),()=>fakeSort([-Infinity]),()=>fakeSort(['1']),()=>fakeSort(new Int32Array(2)),()=>fakeSort([1,2],{scratch:new Int32Array(2)}),()=>fakeSort([1,2],{scratch:[]}),()=>{let a=[1,2];fakeSort(a,{scratch:a})},()=>{let a=new Float64Array(4);fakeSort(a.subarray(0,2),{scratch:a.subarray(2)})},()=>fakeSort(Array(2))];for(const f of bad){assert.throws(f);counts.rejected=(counts.rejected||0)+1;}
console.log(process.version,JSON.stringify(counts));console.log('PASS',Object.values(counts).reduce((a,b)=>a+b,0));
const a=[1];console.log('short input invalid scratch accepted:',fakeSort(a,{scratch:a})===a);
const b=[2,1];try{fakeSort(b,{scratch:Object.freeze([0,0])});console.log('frozen scratch output:',b)}catch(e){console.log('frozen scratch throws:',e.message)}
