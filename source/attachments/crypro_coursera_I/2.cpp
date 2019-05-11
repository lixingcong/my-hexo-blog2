#include <iostream>
#include <gmp.h>
#include <cstring>
using namespace std;
char str_N[400]="648455842808071669662824265346772278726343720706976263060439070378797308618081116462714015276061417569195587321840254520655424906719892428844841839353281972988531310511738648965962582821502504990264452100885281673303711142296421027840289307657458645233683357077834689715838646088239640236866252211790085787877";

int main() {
	time_t start_time,end_time;
	time(&start_time);			// 计时开始
	mp_exp_t mp_exp_t_exp;
	size_t n_digits=1200;			// 指定输出str时的有效数字,单位：字节
	mp_bitcnt_t prec=1200;
	char res_p[500],res_q[500];
	mpf_set_default_prec(prec);
	
	mpf_t mpf_t_a,mpf_t_b;		// 浮点数
	mpz_t mpz_t_a,mpz_t_b,mpz_t_c,mpz_t_d; // 整数temp
	mpz_t mpz_t_N;				   // 整数：常量A和N
	mpz_t mpz_t_for_begin,mpz_t_for_end,mpz_t_for_now;   // for循环
	mpf_init(mpf_t_a);
	mpf_init(mpf_t_b);
	mpz_init_set_ui(mpz_t_a,2); // 临时使用
	mpz_init(mpz_t_b);
	mpz_init(mpz_t_c);
	mpz_init(mpz_t_d);
	mpz_init(mpz_t_for_end);
	mpz_init(mpz_t_for_now);
	mpz_init(mpz_t_for_begin);
	mpz_init_set_str(mpz_t_N,str_N,10);
	mpf_init_set_str(mpf_t_b,str_N,10);

	mpf_sqrt(mpf_t_a,mpf_t_b);		  // sqrt(N)存入mpf_a
	mpf_ceil(mpf_t_b,mpf_t_a);		  // ceil后得到浮点数存入mpf_b
	mpz_set_f(mpz_t_for_begin,mpf_t_b);		  // 将ceil后浮点数转成整数存入mpz_for_begin
	mpz_pow_ui(mpz_t_b,mpz_t_a,19);			  // 计算pow(2,19)存入mpz_b
	mpz_add(mpz_t_for_end,mpz_t_for_begin,mpz_t_b); // 计算终止条件存入end
	mpz_set(mpz_t_for_now,mpz_t_for_begin);			// 将now赋初值

	bool isfound=false;

	while(1){
		// A的值从mpz_t_for_begin一直自增到mpz_t_for_end，A取值为mpz_t_for_now
		mpz_pow_ui(mpz_t_d,mpz_t_for_now,2);	  // A^2存入mpz_d
		mpz_sub(mpz_t_c,mpz_t_d,mpz_t_N); // A^2-N存入mpz_c
		mpz_sqrt(mpz_t_b,mpz_t_c);		  // x=sqrt(A^2-N)存入mpz_b
		
		mpz_sub(mpz_t_a,mpz_t_for_now,mpz_t_b); // p=A-x存入mpz_a
		mpz_add(mpz_t_c,mpz_t_for_now,mpz_t_b); // q=A+x存入mpz_c
		mpz_mul(mpz_t_d,mpz_t_c,mpz_t_a); // p*q存入mpz_d

		if(mpz_cmp(mpz_t_d,mpz_t_N)==0){  // 找到
			isfound=true;
			break;
		}

		mpz_add_ui(mpz_t_for_now,mpz_t_for_now,1);	 // 自增1
		
		if(mpz_cmp(mpz_t_for_now,mpz_t_for_end)==0){ // 超范围了
			cout<<"exit loop since out of range.\n";
			break;
		}
	}
	if(isfound){
		mpz_get_str(res_p,10,mpz_t_a);
		cout<<"p = "<<res_p<<endl;
		mpz_get_str(res_q,10,mpz_t_c);
		cout<<"q = "<<res_q<<endl;
		if(strcmp(res_p,res_q)<0){
			cout<<"p is smaller"<<endl;
		}else
			cout<<"q is smaller"<<endl;
	}else 
		cout<<"not found!"<<endl;
	return 0;
}
