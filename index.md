---
title       : Rcpp簡介
subtitle    : 
author      : Wush Wu
job         : Taiwan R User Group
framework   : io2012        # {io2012, html5slides, shower, dzslides, ...}
highlighter : highlight.js  # {highlight.js, prettify, highlight}
hitheme     : zenburn       # 
widgets    : [bootstrap, quiz, shiny, interactive, mathjax]
mode        : selfcontained # {standalone, draft}
license: by-nc-sa
logo: Taiwan-R-logo.png
--- .quote .segue .nobackground .dark




<q><span class = 'white'>Rcpp<br/>
</br>
讓我們能同時兼顧開發速度與執行效能
</span></q>


--- &vcenter .large

R 是一個很棒的工具

大幅縮短開發資料分析應用的時間

--- .large &vcenter

但是R 也常常被別人詬病效能問題

--- &vcenter .large

C++ 也是一個很棒的工具

在效能的控制上非常突出

--- &vcenter .large

但是C++ 的開發難度高很多

--- &vcenter .large

Rcpp 

讓我們在開發上，

能兼用兩者的優點

--- &twocol

## 效能比較

*** =left

### R


```r
fibonacciR <- function(n) {
  if (n == 0) return(0)
  if (n == 1) return(1)
  fibonacciR(n-1) + 
    fibonacciR(n - 2)
}
```


*** =right

### Rcpp


```cpp
#include <Rcpp.h>

using namespace Rcpp;

//[[Rcpp::export]]
int fibonacciRcpp(int n) {
  if (n == 0) return 0;
  if (n == 1) return 1;
  return fibonacciRcpp(n-1) + 
    fibonacciRcpp(n-2);
}
```


---

## 直接PK了！


```r
library(rbenchmark)
benchmark(
  fibonacciR(10),
  fibonacciRcpp(10)
  )
```

```
               test replications elapsed relative user.self sys.self user.child sys.child
1    fibonacciR(10)          100   0.025       25     0.025    0.001          0         0
2 fibonacciRcpp(10)          100   0.001        1     0.001    0.001          0         0
```


Rcpp 快了許多倍！

--- &twocol

## 回頭比較一下程式碼

*** =left

### R


```r
fibonacciR <- function(n) {
  if (n == 0) return(0)
  if (n == 1) return(1)
  fibonacciR(n-1) + 
    fibonacciR(n - 2)
}
```


*** =right

### Rcpp


```cpp
#include <Rcpp.h>

using namespace Rcpp;

//[[Rcpp::export]]
int fibonacciRcpp(int n) {
  if (n == 0) return 0;
  if (n == 1) return 1;
  return fibonacciRcpp(n-1) + 
    fibonacciRcpp(n-2);
}
```


--- &vcenter .large

把剛剛右半邊的Rcpp的程式碼存到`fibonacci.cpp`

--- &interactive

## 短短一行...


```r
library(Rcpp)
sourceCpp("fibonacci.cpp")
fibonacciRcpp(10)
```

[1] 55


--- &vcenter .large

Rcpp Attribute

在註解中自動將C++函數暴露到R環境中

---

## Rcpp Attribute


```cpp
//[[Rcpp::depends(Matrix, RcppArmadillo)]]

#include <Rcpp.h>

using namespace Rcpp;

//[[Rcpp::export]]
```


- 利用註解`/[[Rcpp::export]]`來將指定的C++函數暴露到R
- 利用`sourceCpp`指令直接將`.cpp`的檔案匯入R
- `Rcpp::depends`用來指定其他建置相依性
    - `[[Rcpp::depends(Matrix, RcppArmadillo)]]`

--- &vcenter .large

來個Hello World吧！


```cpp
#include <Rcpp.h>

using namespace Rcpp;

//[[Rcpp::export]]
void hello() {
  std::cout << "Hello World" << std::endl;
}
```


--- .segue .dark

## 型別轉換和基礎知識

--- 

## Automic 形態

<!-- html table generated in R 3.0.3 by xtable 1.7-3 package -->
<!-- Thu May  8 23:54:36 2014 -->
<TABLE border=1>
<TR> <TH>  </TH> <TH> R </TH> <TH> C++ </TH> <TH> Rcpp </TH> <TH> C API of R </TH>  </TR>
  <TR> <TD align="right"> 1 </TD> <TD> integer </TD> <TD> int </TD> <TD> IntegerVector </TD> <TD> INTEGER </TD> </TR>
  <TR> <TD align="right"> 2 </TD> <TD> numeric </TD> <TD> double </TD> <TD> NumericVector </TD> <TD> NUMERIC </TD> </TR>
  <TR> <TD align="right"> 3 </TD> <TD> character </TD> <TD> std::string </TD> <TD> CharacterVector </TD> <TD> CHAR, STRING_ELT </TD> </TR>
  <TR> <TD align="right"> 4 </TD> <TD> logical </TD> <TD> bool </TD> <TD> LogicalVector </TD> <TD> LOGICAL </TD> </TR>
  <TR> <TD align="right"> 5 </TD> <TD> NULL </TD> <TD> void </TD> <TD> R_NilValue </TD> <TD> R_NilValue </TD> </TR>
   </TABLE>


--- &twocol

## Integer 

*** =left

### Rcpp


```cpp
#include <Rcpp.h>

using namespace Rcpp;

//[[Rcpp::export]]
int plusRcpp(int a, int b) {
  return a + b;
}
```


*** =right

### C API of R


```cpp
#include <R.h>
#include <Rdefines.h>

SEXP plusC(SEXP a, SEXP b) {
  PROTECT(a = AS_INTEGER(a));
  PROTECT(b = AS_INTEGER(b));
  int *pa = INTEGER_POINTER(a),
    *pb = INTEGER_POINTER(b);
  SEXP retval;
  PROTECT(retval = NEW_INTEGER(1));
  int *pretval = INTEGER_POINTER(retval); 
  pretval[0] = pa[0] + pb[0];
  UNPROTECT(3);
  return retval;
}
```


---

## C的API做了什麼？

- `SEXP`表示Simple EXPression，代表任何形態的R 物件
- 需要利用`PROTECT`來告訴R 的gc對應的`SEXP`物件在使用中，不要回收
- 結束時需要用`UNPROTECT(n)`來告訴gc，最後`n`個`PROTECT`的物件已經沒在用了
- 利用`INTEGER`等API來把`SEXP`轉換成正確的指標

--- &vcenter .large

Rcpp利用OO的技術(ex: RAII)把所有上述的動作都封裝了

--- &twocol

## 結果

*** =left


```r
plusRcpp(1, 2)
```

```
[1] 3
```


*** =right


```r
system("R CMD SHLIB integerC.c")
dyn.load("integerC.so")
.Call("plusC", 1L, 2L)
```

```
[1] 3
```

```r
dyn.unload("integerC.so")
```


---

## 動態連結

- R中可以利用動態連結做功能擴充
- `.Call`函數讓使用者可以在R中呼叫C的函數
- `dyn.load`把編譯好的動態函式庫載入

<br/>

<div class='centered'>
**這些動作在Rcpp之中利用Rcpp Attribute都自動做完**
</div>

<br/>



```r
plusRcpp
```

```
function (a, b) 
.Primitive(".Call")(<pointer: 0x1037612d4>, a, b)
```


--- &twocol

## Rcpp物件

*** =left

### R ==> C++


```cpp
#include <Rcpp.h>

//[[Rcpp::export]]
void RToCpp(SEXP Rsrc) {
  Rcpp::IntegerVector src(Rsrc);
  int sum = 0;
  for(int i = 0;i < src.size();i++) {
    sum += src[i];
  }
  Rcpp::Rcout << sum << std::endl;
}
```



```r
RToCpp(1:10)
```

```
55
```


*** =right

### C++ ==> R


```cpp
#include <Rcpp.h>

//[[Rcpp::export]]
SEXP CppToR() {
  Rcpp::IntegerVector retval(10);
  for(int i = 0;i < 9;i++) retval[i] = i;
  return retval;
}
```



```r
CppToR()
```

```
 [1] 0 1 2 3 4 5 6 7 8 0
```


--- &twocol

## 和STL的轉換

*** =left

### R => STL: `as`


```cpp
#include <Rcpp.h>

//[[Rcpp::export]]
void sumSTL(SEXP Rsrc) {
  typedef std::vector<double> NumVec;
  NumVec src(Rcpp::as<NumVec>(Rsrc));
  Rcpp::Rcout << std::accumulate(
    src.begin(), 
    src.end(), 0.0) << std::endl;
}
```



```r
sumSTL(1:10)
```

```
55
```


*** =right

### STL => R: `wrap`


```cpp
#include <Rcpp.h>
typedef std::vector<double> NumVec;
typedef std::map<std::string, NumVec> Dict;

//[[Rcpp::export]]
SEXP STLToR() {
  Dict retval;
  for(int i = 0;i < 10;i++) 
    retval["test"].push_back(i);
  return Rcpp::wrap<Dict>(retval);
}
```



```r
STLToR()
```

```
$test
 [1] 0 1 2 3 4 5 6 7 8 9
```


--- &twocol

## 函數

*** =left


```cpp
#include <Rcpp.h>

//[[Rcpp::export]]
SEXP fInCpp(SEXP input) {
  Rcpp::Function f("f");
  return f(input);
}
```


*** =right


```r
f <- function(x) x + 1
fInCpp(1)
```

```
[1] 2
```

```r
f <- function(x) x + 2
fInCpp(1)
```

```
[1] 3
```


f 的形態：

```
SEXP f(SEXP, ...);
```

--- .segue .dark

## 將C++物件直接轉換為R物件

--- &twocol

## Rcpp Modules

- 能夠將C++物件連同內部資料直接暴露在R之中直接作操作

*** =left

### C++ 物件


```cpp
struct Rectangle {
  double width;
  double height;
  double area() const {
    return width * height;
  }
};

#include <Rcpp.h>

RCPP_MODULE(Geometry) {
  Rcpp::class_<Rectangle>("Rectangle")
    .constructor()
    .field("width", &Rectangle::width)
    .field("height", &Rectangle::height)
    .method("area", &Rectangle::area)
    ;
}
```


*** =right

### R 中的物件


```r
Rectangle
```

```
C++ class 'Rectangle' <0x7fbe81434fd0>
Constructors:
    Rectangle()

Fields: 
    double height
    double width

Methods: 
     double area()  const 
           
```


---

## 可以利用R的OO語法建立物件


```r
r <- new(Rectangle)
r$width
```

```
[1] 0
```

```r
r$width <- 10
r$width
```

```
[1] 10
```

```r
r$height <- 20
r$area()
```

```
[1] 200
```


--- .segue .dark

## 前置知識建立完畢！讓我們開Hack吧

--- &vcenter .large

Logistic Regression

$$P( y | x ) = \frac{1}{1 + e^{- y w^T x}}$$

### $y$: 有沒有點擊
### $x$: 相關參數
### $w \in \mathbb{R}^d$: 模型

--- &vcenter .large

手上有這些資料

$$(y_1, x_1), (y_2, x_2), ...(y_n, x_n)$$

--- &vcenter .large

Regularized Loss:

$$f(w) = \frac{1}{2} w^T w + C \sum_{i=1}^n {log(1 + e^{-y_i w^T x_i})}$$

### 定出 $(y_1, x_1), ...$ 之後，
### 理想的模型 $w$ 是讓 $f$ 最小的點

--- &vcenter .large

Hessian矩陣很大 $\nabla^2 f(w) \in \mathbb{R}^{n \times n}$

但是可以改寫成

$$I_n + CX^T D X$$

--- &vcenter .large

只要算 Hessian和vector的乘積 

我們不需要知道Hessian就可以找$w$

--- &vcenter .large

## LIBLINEAR 原始碼中的 `tron.h`


```cpp
class TRON
{
public:
  TRON(const function *fun_obj, double eps = 0.1, int max_iter = 1000);
	~TRON();

	void tron(double *w);
	void set_print_string(void (*i_print) (const char *buf));

private:
	//...
};
```


- LIBLINEAR實作了上述概念
- TRON是最佳化的核心
- TRON沒有牽涉到資料，甚至沒有牽涉到Loss Function `f`
- `*fun_obj`提供了一個界面

--- &vcenter .large

## 界面：`function`


```cpp
class function
{
public:
  virtual double fun(double *w) = 0 ;
	virtual void grad(double *w, double *g) = 0 ;
	virtual void Hv(double *s, double *Hs) = 0 ;

	virtual int get_nr_variable(void) = 0 ;
	virtual ~function(void){}
};
```


- `fun`: $f(w)$
- `grad`: $\nabla f(w)$, `g`是輸出
- `Hv`: $\nabla^2 f(w) s$, `Hs`是輸出, `w`是上一個呼叫`grad`的`w`
- `get_nr_variable`: `w`的長度

--- &twocol

## 把`function`暴露到R中

*** =left

### 實作Rfunction


```cpp
class Rfunction : public ::function {
  Rcpp::Function _fun, _grad, _Hv;
  Rcpp::NumericVector _w, _s;
  
  Rfunction(SEXP fun, SEXP grad, SEXP Hv, int n) 
  : _fun(fun), _grad(grad), _Hv(Hv), _w(n), _s(n) { }
  
  //...
};

SEXP tron//...
```


*** =right

將Rfunction暴露出來


```cpp
RCPP_MODULE(HsTrust) {  
	class_<Rfunction>("HsTrust")
	.constructor<SEXP, SEXP, SEXP, int>()
	.property("n", &Rfunction::get_nr_variable, 
    "Number of parameters")
	.method("tron", &tron)
	.method("tron_with_begin", 
    &tron_with_begin)
	;	
}
```


--- &twocol

*** =left

## 實作的結果

- [HsTrust](https://bitbucket.org/wush978/largescalelogisticregression/src/4daf9c5bba5cd0e4f35afd813866e6da72ca92bb/?at=hstrust) 套件

### 安裝


```r
library(devtools)
install_bitbucket(
  repo="largescalelogisticregression", 
  username="wush978", ref="hstrust")
```


*** =right

### 使用


```r
library(HsTrust)

set.seed(1)
beta <- rnorm(1)
n <- 1
fun <- function(w) 
  sum((w-beta)^4)

grad <- function(w)
	4 * (w-beta)^3

Hs <- function(w, s) 
  12 * (w-beta)^2 * s

obj <- new(HsTrust, fun, grad, Hs, n)
print(r <- obj$tron(1e-4, TRUE))
```


---

## 結果


```
iter  1 act 1.040e+03 pre 8.640e+02 delta 2.512e+00 f 1.296e+03 |g| 8.640e+02 CG   1
iter  2 act 2.054e+02 pre 1.707e+02 delta 2.512e+00 f 2.560e+02 |g| 2.560e+02 CG   1
iter  3 act 4.058e+01 pre 3.371e+01 delta 2.512e+00 f 5.057e+01 |g| 7.585e+01 CG   1
iter  4 act 8.016e+00 pre 6.659e+00 delta 2.512e+00 f 9.989e+00 |g| 2.247e+01 CG   1
iter  5 act 1.583e+00 pre 1.315e+00 delta 2.512e+00 f 1.973e+00 |g| 6.659e+00 CG   1
iter  6 act 3.128e-01 pre 2.598e-01 delta 2.512e+00 f 3.897e-01 |g| 1.973e+00 CG   1
iter  7 act 6.178e-02 pre 5.132e-02 delta 2.512e+00 f 7.699e-02 |g| 5.846e-01 CG   1
iter  8 act 1.220e-02 pre 1.014e-02 delta 2.512e+00 f 1.521e-02 |g| 1.732e-01 CG   1
[1] 5.77
```

