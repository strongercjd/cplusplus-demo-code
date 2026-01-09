# 编译器
CXX = g++
# 编译选项：-fPIC（位置无关代码，用于动态库）、-Wall（警告）、-std=c++11
CXXFLAGS = -fPIC -Wall -std=c++11
# 链接选项：-ldl（链接动态库加载库）
LDFLAGS = -ldl

# 默认目标：构建所有目标  必须先编译libmessage_pub.so
all: libmessage_pub.so  libalert_nodelet.so libjob_nodelet.so  plugin_loader

# 目标1：编译插件动态库
libalert_nodelet.so: alert_nodelet.cpp nodelet_base.hpp
	$(CXX) $(CXXFLAGS) -shared -o $@ $<  -L./ -lmessage_pub  

libjob_nodelet.so: job_nodelet.cpp nodelet_base.hpp
	$(CXX) $(CXXFLAGS) -shared -o $@ $< 

# 目标2：编译插件加载器
plugin_loader: plugin_loader.cpp nodelet_base.hpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS) 

libmessage_pub.so: message_pub.cpp message_pub.hpp
	$(CXX) $(CXXFLAGS) -shared -o $@ $<

# 清理编译产物
clean:
	rm -f *.so plugin_loader