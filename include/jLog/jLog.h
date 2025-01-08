#ifndef jLOG_H
#define jLOG_H

/*

      jLog a quick and dirty header only logging library 

*/

// MIT License

// Copyright (c) 2024 Jerboa

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.


#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

#include <ctime>

#ifndef ANDROID
#else
  #include <android/log.h>
#endif

namespace jLog
{
  
  inline std::string get_time()
  {
    time_t time = std::time(nullptr);
    char time_buf[80];
    struct tm ts;
    ts = *localtime(&time);
    strftime(time_buf, sizeof(time_buf), "%a %Y-%m-%d %H:%M:%S %Z", &ts);

    return std::string(time_buf);
  }

  class Log 
  {
  
  public:
  
    Log(const char * s){
      logStream.clear();
      logStream.push_back(std::string(s));
    }

    Log(){}
    
    ~Log(){
      if (logStream.size()>0){
        #if defined(ANDROID) || defined(WINDOWS)
          put("Destructor called on non-empty log, contents:\n\n");
        #else
          put("\033[1;33m[WARN] \033[0mDestructor called on non-empty log, contents:\n\n");
        #endif

        #if !defined(ANDROID)
          std::cout << get();
        #else
          __android_log_print(ANDROID_LOG_WARN, "", "%s", get().c_str());
        #endif
      }
    }

    #if defined(ANDROID)
      void androidLog(){ if(logStream.size()>0){__android_log_print(ANDROID_LOG_INFO,"Hop","%s",get().c_str());} }
    #endif

    void put(std::string s){
      s = get_time() + " " + s;
      logStream.push_back(s);
    }

    inline void put(const char * c){
      std::string s = get_time() + " " + std::string(c);
      logStream.push_back(s);
    }

    inline std::string get() {
      std::string l;
      for (unsigned i = 0; i < logStream.size(); i++){
        l += logStream[i] + "\n";
      }
      logStream.clear();
      return l;
    }

    inline size_t size(){return logStream.size();}

  private:
    std::vector<std::string> logStream;
  };

  inline std::ostream & operator<<(std::ostream & o, Log & l)
  {
    o << l.get();
    return o;
  }

  class LogType {
  public:
    LogType(){}
    LogType(std::string s):msg(s){}
    LogType(const char * c):msg(c){}
    virtual std::string get()const {return std::string("");};
    void operator>> (Log & l){l.put(this->get());}
  protected:
    std::string msg;
  };

  /*
    INFO log type leaves a [INFO] declarator and a msg
  */

  class INFO : public LogType{
  public:
    using LogType::LogType; // inherit all constructors
    std::string get()const{return u+msg;}
  private:
    #if !defined(ANDROID) && !defined(WINDOWS)
      const char * u = "\033[1;36m[INFO] \033[0m";
    #else
      const char * u = "[INFO] ";
    #endif
  };

  /*
    WARN log type leaves a [WARN] declarator and a msg
  */

  class WARN : public LogType{
  public:
    using LogType::LogType; // inherit all constructors
    std::string get()const{return u+msg;}
  private:
    #if !defined(ANDROID) && !defined(WINDOWS)
      const char * u = "\033[1;33m[WARN] \033[0m";
    #else
      const char * u = "[WARN] ";
    #endif

  };

  /*
    ERR log type leaves a [ERR] declarator follwed by a (ERRORCODE) and
    finally a msg
  */

  enum ERRORCODE { UNSPECIFIED, LUA_ERROR};

  inline std::string operator+ (std::string s, const ERRORCODE e)
  {
    switch (e) 
    {
      case ERRORCODE::UNSPECIFIED:
        return s+std::string("UNSPECIFIED");
        break;
      case ERRORCODE::LUA_ERROR:
        return s+std::string("LUA");
        break;
      default:
        return s+std::string("UNSPECIFIED");
    }
  }

  class ERR : public LogType{
  public:
    ERR(std::string s) : LogType(s) {}
    ERR(const char * c) : LogType(c) {}
    ERR(ERRORCODE xx, std::string s) : LogType(s), x(xx) {}
    ERR(ERRORCODE xx, const char * c) : LogType(c), x(xx) {}
    std::string get()const{return u+std::string("\033[1;33m(")+x+std::string(")\033[0m ")+msg;}
  private:
    const char * u = "\033[1;31m[ERR] \033[0m";
    ERRORCODE x = ERRORCODE::UNSPECIFIED;
  };

  /*
    PROGRESS log type leaves a msg followed by a progress meter
  */

  class Progress {
  public:
    Progress(
      uint64_t l,
      uint64_t p,
      std::vector<std::string> s = std::vector<std::string>{"\\","-","/","-"}
    )
    : length(l), progress(p), sequence(s)
    {}
    bool next(std::ostream & o){
      o << '\r';
      o.flush();
      if (progress < length){
        progress++;
        o << '\t' << sequence[progress % sequence.size()];
        return false;
      }
      else {
        return true;
      }
    }
  private:
    uint64_t length;
    uint64_t progress;
    std::vector<std::string> sequence;
  };
}
#endif /* jLOG_H */
