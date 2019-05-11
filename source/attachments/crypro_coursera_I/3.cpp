#include <iostream>
#include <gmp.h>
#include <cstring>
using namespace std;

char str_N[400]="720062263747350425279564435525583738338084451473999841826653057981916355690188337790423408664187663938485175264994017897083524079135686877441155132015188279331812309091996246361896836573643119174094961348524639707885238799396839230364676670221627018353299443241192173812729276147530748597302192751375739387929";

int main() {
	mp_exp_t mp_exp_t_exp;
	size_t n_digits=1200;			// 指定输出str时的有效数字,单位：字节
	mp_bitcnt_t prec=1200;
	char res_p[500],res_q[500];
	mpf_set_default_prec(prec);
	
	mpf_t mpf_t_a,mpf_t_b;		// 浮点数
	mpz_t mpz_t_a,mpz_t_b,mpz_t_c,mpz_t_d; // 整数
	mpz_t mpz_t_N,mpz_t_A,mpz_t_Nx;				   // 整数：常量A和N
	mpf_init(mpf_t_a);
	mpf_init(mpf_t_b);
	mpz_init(mpz_t_a);
	mpz_init(mpz_t_A);
	mpz_init(mpz_t_b);
	mpz_init(mpz_t_c);
	mpz_init(mpz_t_d);
	mpz_init(mpz_t_Nx);
	mpz_init_set_str(mpz_t_N,str_N,10);
	mpz_mul_ui(mpz_t_Nx,mpz_t_N,24); // N*24存入新的mpz_N
	mpf_set_z(mpf_t_b,mpz_t_Nx);
	
	mpf_sqrt(mpf_t_a,mpf_t_b);		  // sqrt(24N)存入mpf_a
	mpf_ceil(mpf_t_b,mpf_t_a);		  // ceil后得到浮点数存入mpf_b
	mpz_set_f(mpz_t_A,mpf_t_b);		  // 将ceil后浮点数转成整数存入mpz_A
	mpz_pow_ui(mpz_t_d,mpz_t_A,2);	  // A^2存入mpz_d
	mpz_sub(mpz_t_c,mpz_t_d,mpz_t_Nx); // A^2-24N存入mpz_c
	mpz_sqrt(mpz_t_b,mpz_t_c);		  // x=sqrt(A^2-24N)存入mpz_b

	mpz_sub(mpz_t_a,mpz_t_A,mpz_t_b); // (A-x)存入mpz_a
	mpz_div_ui(mpz_t_a,mpz_t_a,6);	  // p=(A-x)/6存入mpz_a
	mpz_add(mpz_t_c,mpz_t_A,mpz_t_b); // (A+x)存入mpz_c
	mpz_div_ui(mpz_t_c,mpz_t_c,4);	  // q=(A+x)/4存入mpz_c
	
	mpz_get_str(res_p,10,mpz_t_a);
	cout<<"p = "<<res_p<<endl;
	mpz_get_str(res_q,10,mpz_t_c);
	cout<<"q = "<<res_q<<endl;
	if(mpz_cmp(mpz_t_a,mpz_t_c)>0){
		cout<<"q is smaller\n";
	}else
		cout<<"p is smaller\n";

	return 0;
}
