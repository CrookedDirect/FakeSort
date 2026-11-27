// Fake Sort: finite numeric arrays; modifies and returns input.
const networks = {"2": [[1, 0]], "3": [[1, 0], [2, 1], [1, 0]], "4": [[1, 0], [3, 2], [2, 0], [3, 1], [2, 1]], "5": [[3, 0], [4, 1], [2, 0], [3, 1], [1, 0], [4, 2], [2, 1], [4, 3], [3, 2]], "6": [[5, 0], [3, 1], [4, 2], [2, 1], [4, 3], [3, 0], [5, 2], [1, 0], [3, 2], [5, 4], [2, 1], [4, 3]], "7": [[6, 0], [3, 2], [5, 4], [2, 0], [4, 1], [6, 3], [1, 0], [5, 2], [4, 3], [2, 1], [6, 4], [3, 2], [5, 4], [2, 1], [4, 3], [6, 5]], "8": [[2, 0], [3, 1], [6, 4], [7, 5], [4, 0], [5, 1], [6, 2], [7, 3], [1, 0], [3, 2], [5, 4], [7, 6], [4, 2], [5, 3], [4, 1], [6, 3], [2, 1], [4, 3], [6, 5]]};
function fakeSort(a,{mode="optimized",scratch=null}={}){
 if(!["original","pure","advanced","optimized"].includes(mode))throw Error("unknown mode");
 if(!Array.isArray(a)&&!(a instanceof Float64Array))throw Error("Array or Float64Array required");
 for(const v of a)if(!Number.isFinite(v))throw Error("finite numbers only");
 const n=a.length;if(n<2)return a;
 const original=mode==="original", c8=mode==="advanced"||mode==="optimized";
 const b=scratch??new Array(n);if((!Array.isArray(b)&&!(b instanceof Float64Array))||b===a||b.length<n|| (ArrayBuffer.isView(a)&&ArrayBuffer.isView(b)&&a.buffer===b.buffer))throw Error("invalid scratch");
 function small(src,l,k){const v=Array.from({length:k},(_,i)=>src[l+i]);for(const [i,j] of networks[k])if(v[i]<v[j])[v[i],v[j]]=[v[j],v[i]];for(let i=0;i<k;i++)a[l+i]=v[i];}
 if(c8&&n<=8){small(a,0,n);return a;}
 let rooms=[[0,n-1,false,false]],next=[];
 while(rooms.length){next.length=0;for(const [l,r,flag,parity] of rooms){
 const src=parity?b:a,dst=parity?a:b,pi=l+Math.floor((r-l)/(flag?4:2)),p=src[pi];let w=l,z=r;
 function low(i){const v=src[i];if(v<p)dst[w++]=v;else if(v>p)dst[z--]=v;}
 if(original){for(let i=l;i<=r;i++)low(i);}else{for(let i=l;i<=pi;i++)low(i);for(let i=r;i>pi;i--){const v=src[i];if(v>p)dst[z--]=v;else if(v<p)dst[w++]=v;}}
 if(original||parity||w!==l||z!==r)for(let i=w;i<=z;i++)a[i]=p;
 function child(x,y){const k=y-x+1;if(k<1)return;if(k===1){if(!parity)a[x]=dst[x];return;}if(c8&&k<=8){small(dst,x,k);return;}next.push([x,y,!original&&8*k>7*(r-l+1),!parity]);}
 child(l,w-1);child(z+1,r);
 }if(mode==="optimized"){[rooms,next]=[next,rooms];}else{rooms=next;next=[];}}return a;
}
if(typeof module!=="undefined")module.exports={fakeSort};
