# Pointers on C++'s smart pointers

> Sorry for the title, I just couldn't help myself.

## TL;DR

Don't use smart pointers in function parameters unless you're taking
ownership (shared or exclusive).

Return stack objects instead of smart pointers when possible.

```c++
// bad
int count(const std::shared_ptr<std::string> &s, char d);

// good
int count(const std::string &s, char d);
```

Avoid using pointers, the following is the preference for creating
objects:

| Pointer type           | Comment                                                                                                                         |
|------------------------|---------------------------------------------------------------------------------------------------------------------------------|
| `T` (stack allocation) | Best option                                                                                                                     |
| `unique_ptr<T>`        | If you really need a stack allocation                                                                                           |
| `shared_ptr<const T>`  | Not great, but sometimes necessary                                                                                              |
| `T *`                  | When working with C code, shouldn't be exposed long [I.11](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Ri-raw) |
| `shared_ptr<T>`        | Be careful with this one, avoid                                                                                                 |


## Smart pointers as parameters

Let's take the example from the TL;DR, and explain why having smart
pointers in the interface is a bad idea.

```c++
// bad
int count_bad(const std::shared_ptr<std::string> &s, char d);

// good
int count_good(const std::string &s, char d);
```

The first thing to mention is that they are both easily callable if I
have a smart pointer object.

```c++
auto shared = std::make_shared<std::string>("Hello shared world!");

int bad = count_bad(shared, 'l');

// Calling the non-smart pointer is as easy as adding a *
int good = count_good(*shared, 'l');
```

They're both as easy to call but taking a smart pointer as a parameter
puts pressure on the caller to store their objects in a smart
pointer. In the worst case, you end up putting all your objects in
smart pointers even when they don't have to be to make calling
functions easier.

When you use smart pointers, you're leaving performance on the table —
you rule out faster options like using the stack or arena
alocation. You don't always need the performance but it's nice to be
able to get it when you need it.

There's also a link between your function parameter and how I store my
object: you take `shared_ptr` but I want to use a `unique_ptr`. I have
the option to rewrap the object, use a `shared_ptr` or update the
function to use a `unique_ptr` — none of these are good choices.

The intention of the function also becomes unclear. Are we only
reading the parameter or taking a copy for future use? With a `const
shared_ptr`, both are possible.

I hope I've convinced you that if you only need to read a parameter of
type `T`, use `const T &` instead of `const shared_ptr<T> &` — only
take `const shared_ptr<T> &` if you are taking a copy of the shared
pointer for future use. There are no downsides but many upsides:

- easier to call,
- faster when needed,
- decoupled code,
- clearer intention.

All of this to avoid having to use `operator*`!

## Const correctness

If you aren't yet convinced, the bad version of the interface doesn't
give you the `const` protections you might expect:

```c++
auto shared = std::make_shared<std::string>("Hello shared world!");

printf("Good count\n");
int good = count_good(*shared, 'l');
printf("'%s' contains %i 'l's.\n\n", shared->c_str(), good);

printf("Bad count\n");
int bad = count_bad(shared, 'l');
printf("'%s' contains %i 'l's.\n\n", shared->c_str(), bad);
```

Prints

```
Good count
'Hello shared world!' contains 3 'l's.

Bad count
'He**o shared wor*d!' contains 3 'l's.
```

Notice the `*`s where the `l`s used to be for the bad count. The
object that is pointed to in a `const shared_ptr<T>` can be modified!
It's the same with a unique pointer.

For historic reasons, a `const shared_ptr<T>` doesn't protect the `T`
from being modified but a `shared_ptr<const T>` does. I have another
article ([Implementing a const correct shared_ptr](../const-correct-shared-ptr/index.html)) that explores
using newer C++ features to make a const correct side pointer if you
want to dig into those details.

We can still use `const` to our advantage by using the
`shared_ptr<const T>` type to restrict modification. This is a really
useful trick to overcome the `shared_ptr`'s biggest limitation — loss
of control as to where and when it can change.

We're always told to [Avoid non-const global variables (I.2)](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Ri-global) and
the reasoning is:

> Non-const global variables hide dependencies and make the
> dependencies subject to unpredictable changes.

`const shared_ptr<T>` have the same problem as a global variable — any
code that can get a hold of the pointer can make an unpredictable
change.

Let's say we have a race condition on the data held in a `const
shared_ptr<T>`, we have to figure out:

> Which thread modifies the shared pointer?

You can't easily answer that question without auditing the code that
can access the pointer. Imagine having to fix a race condition with a
shared pointer!

`const` global variables don't have the same weakness as the
`non-const` global variables — the equivalent for `shared_ptr` is
`shared_ptr<const T>`.

Shared pointers have an extra trick where you can have a
`shared_ptr<T>` and give everyone else a `shared_ptr<const T>`. It
becomes easier to audit where the data can be modified.

Here's a simple example, we have a configuration that we load from a
file and want to watch for changes. We hand out the configuration to
many parts of our program via a `shared_ptr<const T>` so they get
updates when the configuration changes. In the example, only the
`ConfigWatcher` can update the configuration object and no one else.

```c++
class ConfigWatcher {
public:
  /**
   * By returning a shared_ptr to a const Config, we make sure that
   * we're the only ones that can modify the underlying configuration.
   */
  std::shared_ptr<const Config> get_config() {
    return config;
  }

  /**
   * This could be triggered when the config is updated.
   */
  void update_config() {
    // Code that can modify config can only be in this class.
  }

private:
  std::shared_ptr<Config> config;
};
```

## Conclusion

Hopefully you'll think twice about the need to use smart pointers when
writting C++ — they are a powerful tool be shouldn't be the
default. This is especially true in function interfaces that are much
cleaner when they don't use smart pointers.

When you need to use a `shared_ptr`, consider using a
`shared_ptr<const T>` instead of a `const shared_ptr<T>`.
