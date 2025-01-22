## PocketSphinx exploration

### To Compile

````bash
g++ -o simple simple.cpp  \
    -I./pocketsphinx-5.0.4/include \
    -I./pocketsphinx-5.0.4/build/include \
    -L./pocketsphinx-5.0.4/build \
    -lpocketsphinx -lm -pthread
    ```
````
