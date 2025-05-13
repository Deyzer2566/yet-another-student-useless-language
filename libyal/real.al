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
    pow1 = (a1 >> 23) & ((1<<8)-1);
    pow2 = (a2 >> 23) & ((1<<8)-1);
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
    while(mantisa >= (1<<24)) {
        pow = pow+1;
        mantisa = mantisa >> 1;
    }
    while(!(mantisa & (1<<23))) {
        pow = pow-1;
        mantisa = mantisa << 1;
    }
    return (sign << 31) | (pow << 23) | mantisa&8388607;
}
int __real_sub(int a1, int a2) {
    mantisa1 = a1 & ((1<<23)-1) | (1<<23);
    mantisa2 = a2 & ((1<<23)-1) | (1<<23);
    pow1 = (a1 >> 23) & ((1<<8)-1);
    pow2 = (a2 >> 23) & ((1<<8)-1);
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
    while(mantisa >= (1<<24)) {
        pow = pow+1;
        mantisa = mantisa >> 1;
    }
    while(!(mantisa & (1<<23))) {
        pow = pow-1;
        mantisa = mantisa << 1;
    }
    return (sign << 31) | (pow << 23) | mantisa&8388607;
}
