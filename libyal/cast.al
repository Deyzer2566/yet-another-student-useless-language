int abs(int a) {
    if(a > 0) {
        return a;
    } else {
        return -a;
    }
}
int __int_to_real(int a) {
    if(a == 0) {
        return 0;
    }
    pow = 23;
    mantisa = abs(a);
    sign = a<0;
    while(mantisa >= (1<<24)) {
        mantisa = mantisa >> 1;
        pow = pow + 1;
    }
    while(!(mantisa & (1<<23))) {
        mantisa = mantisa << 1;
        pow = pow - 1;
    }
    if(pow < 0) {
        return 0;
    }
    if(pow > 255) {
        return (sign<<31) | ((1<<31)-1);
    }
    return (sign<<31) | (((pow+127)&((1<<8)-1))<<23) | (mantisa & ((1<<23)-1));
}
int __real_to_int(int a) {
    pow = ((a>>23)&((1<<8)-1)) - 127;
    mantisa = (a & ((1<<23)-1)) | (1<<23);
    sign = (a>>31) & 1;
    if(sign == 1) {
        sign = -1;
    } else {
        sign = 1;
    }
    pow = pow-23;
    if(pow >= 23) {
        return (mantisa<<pow)*sign;
    } else {
        return (mantisa>>(-pow))*sign;
    }
}