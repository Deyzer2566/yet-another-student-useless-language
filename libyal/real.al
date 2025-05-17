int abs(int a) {
    if(a < 0) {
        return -a;
    } else {
        return a;
    }
}
int __real_sum(int a1, int a2) {
    mantisa1 = a1 & ((1<<23)-1) | (1<<23);
    mantisa2 = a2 & ((1<<23)-1) | (1<<23);
    pow1 = (a1 >> 23) & ((1<<8)-1) - 127;
    pow2 = (a2 >> 23) & ((1<<8)-1) - 127;
    sign1 = (a1 >> 31) & 1;
    sign2 = (a2 >> 31) & 1;
    pow = 0;
    if(pow1 > pow2) {
        mantisa2 = mantisa2 >> (pow1-pow2);
        pow = pow1;
    } else {
        mantisa1 = mantisa1 >> (pow2-pow1);
        pow = pow2;
    }
    if(sign1) {
        mantisa1 = -mantisa1;
    }
    if(sign2) {
        mantisa2 = -mantisa2;
    }
    mantisa = mantisa1 + mantisa2;
    sign = mantisa < 0;
    mantisa = abs(mantisa);
    if(mantisa == 0) {
        return 0;
    }
    while(mantisa >= (1<<24)) {
        pow = pow+1;
        mantisa = mantisa >> 1;
    }
    while(!(mantisa & (1<<23))) {
        pow = pow-1;
        mantisa = mantisa << 1;
    }
    pow = pow + 127;
    return (sign << 31) | (pow << 23) | mantisa&8388607;
}

int __real_sub(int a1, int a2) {
    mantisa1 = a1 & ((1<<23)-1) | (1<<23);
    mantisa2 = a2 & ((1<<23)-1) | (1<<23);
    pow1 = (a1 >> 23) & ((1<<8)-1) - 127;
    pow2 = (a2 >> 23) & ((1<<8)-1) - 127;
    sign1 = (a1 >> 31) & 1;
    sign2 = (a2 >> 31) & 1;
    pow = 0;
    if(pow1 > pow2) {
        mantisa2 = mantisa2 >> (pow1-pow2);
        pow = pow1;
    } else {
        mantisa1 = mantisa1 >> (pow2-pow1);
        pow = pow2;
    }
    if(sign1) {
        mantisa1 = -mantisa1;
    }
    if(sign2) {
        mantisa2 = -mantisa2;
    }
    mantisa = mantisa1 - mantisa2;
    sign = mantisa < 0;
    mantisa = abs(mantisa);
    if(mantisa == 0) {
        return 0;
    }
    while(mantisa >= (1<<24)) {
        pow = pow+1;
        mantisa = mantisa >> 1;
    }
    while(!(mantisa & (1<<23))) {
        pow = pow-1;
        mantisa = mantisa << 1;
    }
    pow = pow + 127;
    return (sign << 31) | (pow << 23) | mantisa&8388607;
}

int __real_negation(int a) {
    return a ^ (1<<31);
}

// умножает числа, размер результата которых больше, чем размер слова
// возвращает старшие 32 бита результата
// x,y - числа, k = (32 - битность числа)
int __long_mul_upper(int x, int y, int k) {
    b = 32 - k;
    x_h = (x>>k)&((1<<b)-1);
    y_h = (y>>k)&((1<<b)-1);
    x_l = x&((1<<k)-1);
    y_l = y&((1<<k)-1);
    return ((x_h*y_h)<<k) + x_h*y_l + x_l*y_h + ((x_l*y_l) >> k);
}

int __real_mul(int a1, int a2) {
    mantisa1 = a1 & ((1<<23)-1);
    mantisa2 = a2 & ((1<<23)-1);
    pow1 = ((a1 >> 23) & ((1<<8)-1)) - 127;
    pow2 = ((a2 >> 23) & ((1<<8)-1)) - 127;
    sign1 = (a1 >> 31) & 1;
    sign2 = (a2 >> 31) & 1;
    if(((mantisa1==0) && (pow1==0)) || ((mantisa2 == 0) && (pow2==0))) {
        return (sign1^sign2)<<31;
    }
    if((pow1 == ((1<<8)-1)) || (pow2 == ((1<<8)-1))) {
        return ((sign1^sign2)<<31) | ((1<<31)-1);
    }
    mantisa1 = mantisa1 | (1<<23);
    mantisa2 = mantisa2 | (1<<23);
    mantisa = __long_mul_upper(mantisa1, mantisa2, 48-32)>>(32-24);
    pow = pow1+pow2+127+((mantisa & (1<<23)) != 0);
    sign = sign1 ^ sign2;
    while(!(mantisa & (1<<23))) {
        mantisa = mantisa << 1;
    }
    return (sign << 31) | (pow << 23) | (mantisa&((1<<23)-1));
}

int __long_div(int a1, int a2) {
    r = 0;
    for(i = 0,i<31,i=i+1) {
        r = r << 1;
        if(a1 > a2) {
            a1 = a1 - a2;
            r = r | 1;
        }
        a1 = a1 << 1;
    }
    return r;
}

int __real_div(int a1, int a2) {
    mantisa1 = a1 & ((1<<23)-1);
    mantisa2 = a2 & ((1<<23)-1);
    pow1 = ((a1 >> 23) & ((1<<8)-1)) - 127;
    pow2 = ((a2 >> 23) & ((1<<8)-1)) - 127;
    sign1 = (a1 >> 31) & 1;
    sign2 = (a2 >> 31) & 1;
    if((mantisa1==0) && (pow1==0)) {
        return (sign1^sign2)<<31;
    }
    if((pow1 == ((1<<8)-1)) || (pow2 == ((1<<8)-1)) || ((mantisa2 == 0) && (pow2 == 0))) {
        return ((sign1^sign2)<<31) | ((1<<31)-1);
    }
    mantisa1 = mantisa1 | (1<<23);
    mantisa2 = mantisa2 | (1<<23);
    mantisa = __long_div(mantisa1, mantisa2)>>(31-24);
    pow = pow1-pow2+127;
    sign = sign1 ^ sign2;
    while(mantisa >= (1<<24)) {
        mantisa = mantisa >> 1;
        pow = pow + 1;
    }
    while(!(mantisa & (1<<23))) {
        mantisa = mantisa << 1;
        pow = pow - 1;
    }
    return (sign << 31) | (pow << 23) | (mantisa&((1<<23)-1));
}

int __real_eq(int a1, int a2) {
    return a1 == a2;
}

int __real_neq(int a1, int a2) {
    return a1 != a2;
}

int __real_smaller(int a1, int a2) {
    mantisa1 = a1 & ((1<<23)-1);
    mantisa2 = a2 & ((1<<23)-1);
    pow1 = ((a1 >> 23) & ((1<<8)-1)) - 127;
    pow2 = ((a2 >> 23) & ((1<<8)-1)) - 127;
    sign1 = (a1 >> 31) & 1;
    sign2 = (a2 >> 31) & 1;
    return (sign1 > sign2) || ((sign1 == sign2) && (pow1 < pow2)) || ((sign1 == sign2) && (pow1==pow2) && (mantisa1<mantisa2));
}

int __real_smaller_or_eq(int a1, int a2) {
    return __real_smaller(a1,a2) || __real_eq(a1, a2);
}

int __real_larger(int a1, int a2) {
    mantisa1 = a1 & ((1<<23)-1);
    mantisa2 = a2 & ((1<<23)-1);
    pow1 = ((a1 >> 23) & ((1<<8)-1)) - 127;
    pow2 = ((a2 >> 23) & ((1<<8)-1)) - 127;
    sign1 = (a1 >> 31) & 1;
    sign2 = (a2 >> 31) & 1;
    return (sign1 < sign2) || ((sign1 == sign2) && (pow1 > pow2)) || ((sign1==sign2) && (pow1==pow2) && (mantisa1>mantisa2));
}

int __real_larger_or_eq(int a1, int a2) {
    return __real_larger(a1, a2) || __real_eq(a1, a2);
}
