> A good thing. It means using the keyword const to prevent const
> objects from getting mutated.
>
> -- [isocpp wiki](https://isocpp.org/wiki/faq/const-correctness)

With the above in mind, should the above mutation be possible? That
depends if you consider the object being pointed to as part of the
smart pointer. Most people, including myself, intuitively say yes but
C++'s history says no.

Because of the importance C++ places on backwards compatibility,
changing that is nearly impossible. I explored const correct
`shared_ptr` in another article: .
