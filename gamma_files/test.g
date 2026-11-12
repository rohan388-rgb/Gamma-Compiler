Start

Let number n to 0.
Let number i to 2.
Let boolean prime to true.

Ask "Enter a number" and store in n.

If n is less than 2 {
    Set prime to false.
}

While i times i is less than or equal to n {
    If n remainder i is equal to 0 {
        Set prime to false.
        Stop.
    }
    Set i to i plus 1.
}

If prime {
    Show "Prime".
} Otherwise {
    Show "Not Prime".
}

End