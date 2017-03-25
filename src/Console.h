#ifndef LOWE_UTIL_CONSOLE_
#define LOWE_UTIL_CONSOLE_
#include <string>

/** \file Console.h
    \defgroup logging Logging Macros
    \{

    \def OMPL_ERROR(fmt, ...)
    \brief Log a formatted error string.
    \remarks This macro takes the same arguments as [printf](http://www.cplusplus.com/reference/clibrary/cstdio/printf).

    \def OMPL_WARN(fmt, ...)
    \brief Log a formatted warning string.
    \remarks This macro takes the same arguments as [printf](http://www.cplusplus.com/reference/clibrary/cstdio/printf).

    \def OMPL_INFORM(fmt, ...)
    \brief Log a formatted information string.
    \remarks This macro takes the same arguments as [printf](http://www.cplusplus.com/reference/clibrary/cstdio/printf).

    \def OMPL_DEBUG(fmt, ...)
    \brief Log a formatted debugging string.
    \remarks This macro takes the same arguments as [printf](http://www.cplusplus.com/reference/clibrary/cstdio/printf).

    \}
*/
//SOLVED:: specific datalog(bool) enabled\disenabled  in specific file
// now only set the global logdata flag
//SOLVED:: std::endl in the right time  
//TODO:: add LOWE_TMP
//TODO:: add LOWE_ONCE correct after ONCE the normal one either once ,donot useit!!


#define LOWE_ERROR(fmt, ...)  log(false,__FILE__,__func__, __LINE__, LOG_ERROR, fmt, ##__VA_ARGS__)
#define LOWE_WARN(fmt, ...)   log(false,__FILE__,__func__, __LINE__, LOG_WARN,  fmt, ##__VA_ARGS__)
#define  LOWE_RUN(fmt, ...)   log(false,__FILE__,__func__, __LINE__, LOG_RUN,  fmt, ##__VA_ARGS__)
#define LOWE_INFO(fmt, ...)   log(false,__FILE__,__func__, __LINE__, LOG_INFO,  fmt, ##__VA_ARGS__)
#define LOWE_DEBUG(fmt, ...)  log(false,__FILE__,__func__, __LINE__, LOG_DEBUG, fmt, ##__VA_ARGS__)
#define LOWE_DEBUG_FILE(oh,fmt, ...)  log3(oh,true,__FILE__, __LINE__, LOG_DEBUG, fmt, ##__VA_ARGS__)
#define LOWE_ERROR_FILE(oh,fmt, ...)  log3(oh,true,__FILE__, __LINE__, LOG_ERROR, fmt, ##__VA_ARGS__)
#define LOWE_INFO_FILE(oh,fmt, ...)  log3(oh,true,__FILE__, __LINE__, LOG_INFO, fmt, ##__VA_ARGS__)
#define LOWE_WARN_FILE(oh,fmt, ...)  log3(oh,true,__FILE__, __LINE__, LOG_WARN, fmt, ##__VA_ARGS__)
#define LOWE_RUN_FILE(oh,fmt, ...)      log3(oh,true,__FILE__, __LINE__, LOG_RUN, fmt, ##__VA_ARGS__)
#define LOWE_DEBUG_FILING(oh,fmt, ...)  log3(oh,false,__FILE__, __LINE__, LOG_DEBUG, fmt, ##__VA_ARGS__)
#define LOWE_ERROR_FILING(oh,fmt, ...)  log3(oh,false,__FILE__, __LINE__, LOG_ERROR, fmt, ##__VA_ARGS__)
#define LOWE_INFO_FILING(oh,fmt, ...)  log3(oh,false,__FILE__, __LINE__, LOG_INFO, fmt, ##__VA_ARGS__)
#define LOWE_WARN_FILING(oh,fmt, ...)  log3(oh,false,__FILE__, __LINE__, LOG_WARN, fmt, ##__VA_ARGS__)
#define LOWE_RUN_FILING(oh,fmt, ...)  log3(oh,false,__FILE__, __LINE__, LOG_RUN, fmt, ##__VA_ARGS__)

#define LOWE_INFO_ONCE(fmt, ...)   log(true,__FILE__, __LINE__, LOG_INFO,  fmt, ##__VA_ARGS__)



/** \brief Message namespace. This contains classes needed to
        output error messages (or logging) from within the library.
        Message logging can be performed with \ref logging "logging macros" */
    /** \brief The set of priorities for message logging */
    enum LogLevel
    {
        LOG_DEBUG = 0,
        LOG_INFO,
        LOG_RUN,
        LOG_WARN,
        LOG_ERROR,
        LOG_NONE
    };

    /** \brief Generic class to handle output from a piece of
        code.

        In order to handle output from the library in different
        ways, an implementation of this class needs to be
        provided. This instance can be set with the useOutputHandler
        function. */
    class OutputHandler
    {
    public:

        OutputHandler()
        {
            logdata_ = false;
            endfile_ = false;
            logonce_ = false;
            onceflag_= false;
        }

        virtual ~OutputHandler()
        {
        }

        /** \brief log a message to the output handler with the given text
            and logging level from a specific file and line number */
        virtual void log(const std::string &text, LogLevel level, 
                const char *filename, const char* func , int line) = 0;
        bool logdata_;
        bool endfile_;
        /** \brief user  */
        bool logonce_;
        /** \brief auto  */
        bool onceflag_;
    };

    /** \brief Default implementation of OutputHandler. This sends
        the information to the console. */
    class OutputHandlerSTD : public OutputHandler
    {
    public:

        OutputHandlerSTD(const bool oncelog = false) : OutputHandler()
        {
            logonce_ = oncelog;
        }

        virtual void log(const std::string &text, LogLevel level, 
                const char *filename, const char *func, int line);

    };

    //static OutputHandlerSTD *STD;
    /** \brief Implementation of OutputHandler that saves messages in a file. */
    class OutputHandlerFile : public OutputHandler
    {
    public:

        /** \brief The name of the file in which to save the message data */

        OutputHandlerFile(const char *filename , const bool datalog, const bool oncelog);
        OutputHandlerFile(const char *filename , const bool datalog);
        void constructHandlerFile(const char *filename , const bool datalog, const bool oncelog);

        //{
            //OutputHandlerFile(filename,datalog,false);
        //}

        virtual ~OutputHandlerFile();

        virtual void log(const std::string &text, LogLevel level, 
                const char *filename, const char* func ,  int line);

    private:

        /** \brief The file to save to */
        FILE *file_;

    };

    /** \brief This function instructs navthat no messages should be outputted. Equivalent to useOutputHandler(NULL) */
    void noOutputHandler();

    /** \brief Restore the output handler that was previously in use (if any) */
    void restorePreviousOutputHandler();

    /** \brief Specify the instance of the OutputHandler to use. By default, this is OutputHandlerSTD */
    void useOutputHandler(OutputHandler *oh);

    /** \brief Get the instance of the OutputHandler currently used. This is NULL in case there is no output handler. */
    OutputHandler* getOutputHandler();

    /** \brief Set the minimum level of logging data to output.  Messages
        with lower logging levels will not be recorded. */
    void setLogLevel(LogLevel level);


    //void setDataLog(bool datalog); 


    /** \brief Retrieve the current level of logging data.  Messages
        with lower logging levels will not be recorded. */
    LogLevel getLogLevel();

    /** \brief Root level logging function.  This should not be invoked directly,
        but rather used via a \ref logging "logging macro".  Formats the message
        string given the arguments and forwards the string to the output handler */
    void log(const bool logonce , const char *file, const char* func ,  int line, LogLevel level, const char *m, ...);

    void log2(OutputHandler *oh,const char *file, int line, LogLevel level, const char *m, ...);

    void log3(OutputHandler *oh,const bool endfile, const char *file, int line, LogLevel level, const char *m, ...);

    void logs(std::ofstream &out);



#endif
