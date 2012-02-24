long long f(long long n)

{

    int k;

    long long e, m = n, r;

    if(n <= 1) return n;

    for(k=0, e=1; m >= 10; k++, e *= 10) m /= 10;

    r = n - m*e;

    if(m == 1)

        return(e/10*k+r+1+f(r));

    else

        return(e/10*k*m+e+f(r));

}



int digit_num(long long n)

{

    int k;

    for(k=0; n > 0; k++) n /= 10;

    return k;

}



int main()
{

    int k;

    long long n, v;

    for(n = 1; n<10000000000;)

    {

        v = f(n);

        if(v == n) {

            printf("%lld\n", n);

            n++;

        }

        else if(v > n) n = v;

        else {

            k = digit_num(n + n - v);

            n += (n-v)/k + 1;

        }

    }

}



