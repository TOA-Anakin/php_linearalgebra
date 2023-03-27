# php_linearalgebra

PHP extension in C++ for solving SLEs.

## Build

```
$ phpize
$ ./configure
$ make
$ sudo make install
```

## Configuration

In `php.ini`, allow this extension:

```
extension=linearalgebra.so
```


## PHP localhost server & testing

```
$ php -S localhost:1234 -t www
```

http://localhost:1234/test.php