#include <iostream>
#include <gmp.h>
#include <cstring>


using namespace std;

char str_N[311]="179769313486231590772930519078902473361797697894230657273430081157732675805505620686985379449212982959585501387537164015710139858647833778606925583497541085196591615128057575940752635007475935288710823649949940771895617054361149474865046711015101563940680527540071584560878577663743040086340742855278549092581"; // 309digits

int main() {
	mp_exp_t mp_exp_t_exp;
	size_t n_digits=1200;			// 指定输出str时的有效数字,单位：字节
	mp_bitcnt_t prec=1200;
	char res_p[500],res_q[500];
	mpf_set_default_prec(prec);
	
	mpf_t mpf_t_a,mpf_t_b;		// 浮点数
	mpz_t mpz_t_a,mpz_t_b,mpz_t_c,mpz_t_d; // 整数
	mpz_t mpz_t_N,mpz_t_A;				   // 整数：常量A和N
	mpf_init(mpf_t_a);
	mpf_init(mpf_t_b);
	mpz_init(mpz_t_a);
	mpz_init(mpz_t_A);
	mpz_init(mpz_t_b);
	mpz_init(mpz_t_c);
	mpz_init(mpz_t_d);
	mpz_init_set_str(mpz_t_N,str_N,10);
	mpf_init_set_str(mpf_t_b,str_N,10);
	
	mpf_sqrt(mpf_t_a,mpf_t_b);		  // sqrt(N)存入mpf_a
	mpf_ceil(mpf_t_b,mpf_t_a);		  // ceil后得到浮点数存入mpf_b
	mpz_set_f(mpz_t_A,mpf_t_b);		  // 将ceil后浮点数转成整数存入mpz_A
	mpz_pow_ui(mpz_t_d,mpz_t_A,2);	  // A^2存入mpz_d
	mpz_sub(mpz_t_c,mpz_t_d,mpz_t_N); // A^2-N存入mpz_c
	mpz_sqrt(mpz_t_b,mpz_t_c);		  // x=sqrt(A^2-N)存入mpz_b
	mpz_sub(mpz_t_a,mpz_t_A,mpz_t_b); // p=A-x存入mpz_a
	mpz_add(mpz_t_c,mpz_t_A,mpz_t_b); // q=A+x存入mpz_c
	
	// mpz_mul(mpz_t_d,mpz_t_c,mpz_t_a); // p*q存入mpz_d
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
