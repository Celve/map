import os

name_list = ['one', 'two', 'three', 'four', 'five']
for i in range(5):
    name_list.append(name_list[i] + '.memcheck')
name_str = ""
for name in name_list:
    name_str += " data/{name}".format(name=name)
os.system("echo" + name_str + " | xargs -n 1 cp -v src/map.hpp")
os.system("echo" + name_str + " | xargs -n 1 cp -v src/exceptions.hpp")
os.system("echo" + name_str + " | xargs -n 1 cp -v src/utility.hpp")

for name in name_list:
    os.system("g++ data/{name}/code.cpp -o code 1> log/log-{name}-1.txt 2> log/log-{name}-2.txt".format(name=name))
    os.system("./code > data/{name}/answer.out".format(name=name))
    if name.endswith(".memcheck"):
        os.system("valgrind ./code > log/log-{name}-3.txt".format(name=name))
    print("check {name}".format(name=name))
    os.system("diff -q data/{name}/answer.txt data/{name}/answer.out".format(name=name))
    print("finish check {name}".format(name=name))

for name in name_list:
    os.system("rm data/{name}/map.hpp".format(name=name))
    os.system("rm data/{name}/exceptions.hpp".format(name=name))
    os.system("rm data/{name}/utility.hpp".format(name=name))
    os.system("rm data/{name}/answer.out".format(name=name))

os.system("rm code")
