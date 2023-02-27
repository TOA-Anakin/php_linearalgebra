# linear_algebra

```
sudo apt install php-dev
```
 
To build and install the extension, you can run the following commands in the extension directory:
```
phpize
./configure --enable-linear_algebra
make
sudo make install
```

Once the extension is installed, you can use the `solve_linear_system` function in your PHP scripts like this:
```php
$a = [[1, 2], [3, 4]];
$b = [5, 6];
$x = solve_linear_system($a, $b);
print_r($x);  // prints Array([0] => -4, [1] => 4.5)
```