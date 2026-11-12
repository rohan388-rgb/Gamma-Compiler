Start

Let number n to 0.
Let number original to 0.
Let number reverse to 0.
Let number digit to 0.

Ask "Enter a number" and store in n.

Set original to n.

While n is greater than 0 {
    Set digit to n remainder 10.
    Set reverse to reverse times 10 plus digit.
    Set n to n divided by 10.
}

If reverse is equal to original {
    Show "Palindrome".
} Otherwise {
    Show "Not Palindrome".
}

End