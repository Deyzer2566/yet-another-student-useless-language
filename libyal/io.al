int print_int(int a) {
    addr = "012345678901234567890123456789";//статический буфер под строку
    i = 0;
    if(a == 0) {
        *addr = *"0";
        i = 1;
    }
    while(a > 0) {
        *(addr+i) = (a%10)+*"0";
        i = i + 1;
        a = a/10;
    }
    *(addr+i) = 0;
    for(j=0,j<i/2,j=j+1) {
        c = *(addr+j);
        *(addr+j) = *(addr+i-j-1);
        *(addr+i-j-1) = c;
    }
    print_str(addr);
}
int print_str(int ptr) {
    while(*ptr != 0) {
        print(*ptr);
        ptr = ptr+1;
    }
}