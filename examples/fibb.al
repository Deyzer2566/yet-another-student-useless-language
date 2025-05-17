//use io.al

int main() {
    c = 1;
    p = 1;
    count = 10;
    for(i=2,i<count,i=i+1) {
        n = c+p;
        p = c;
        c = n;
    }
    print_int(count);
    print_str(" число Фибоначчи ");
    print_int(c);
}
