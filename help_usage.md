### Create a new project and use the simple_http_server as submodule...

in your main project create folder put the name you want I will put 
external. then 

#### Step 1. Add the Submodule.
'''
   git submodule add https://github.com/aneury1/simple_http_server.git external/simple_http_server
   git submodile update --init --recursive
'''
after doing previous step you would see .gitsubmodule file configure.


#### Step 2. Configure your project.

go to CMakeLists.txt of your project and set some variables(you can customize)
'''
set(SUBMODULE_PATH ${CMAKE_SOURCE_DIR}/external/SubmoduleProject)
...
add_subdirectory(${SUBMODULE_PATH})
...
target_link_libraries(MainProject PRIVATE simple_http_server)
...
target_include_directories(MainProject PRIVATE ${SUBMODULE_PATH}/include)
'''

#### Step 3. Build and Test
'''
 mkdir build
 cd build 
 cmake .. 
 cmake --build . 
 ...
 ./test_your_app
'''

#### Step 4. Give us Feedback...

please tell us from issue if you need something or if you found some issue or improvements.

#### ...
