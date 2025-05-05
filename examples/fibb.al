int main() {
    f = 1;
    s = 1;
    order = 1;
    count = 15;
    print("123");
    for(i=1,i<count,i=i+1) {
        c = f+s;
        if(order == 1) {
            f = c;
            order = 2;
        } else {
            s = c;
            order = 1;
        }
    }
    print(count," число Фиббоначи ", f,s);
}
