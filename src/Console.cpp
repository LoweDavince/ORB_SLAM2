#include "Console.h"
#include <boost/thread/mutex.hpp>
#include <iostream>
#include <cstdio>
#include <cstdarg>
#define BLD_RED     "\e[1;31m"
#define BLD_GREEN   "\e[1;32m"
#define BLD_YELLOW  "\e[1;33m"
#define BLD_PURPLE  "\e[1;35m"
#define BLD_WHITE   "\e[1;37m"
#define BLD_BLUE    "\e[1;34m"


#define CLR_WHITE   "\e[0;37m"

#define CLR_RESET   "\e[0m"
#define ENABLE_LINE true

/// @cond IGNORE

struct DefaultOutputHandler
{
    DefaultOutputHandler()
    {
        output_handler_ = static_cast<OutputHandler*>(&std_output_handler_);
        previous_output_handler_ = output_handler_;
        logLevel_ = LOG_INFO;
        //logdata_  = false;
        //endfile_  = false;
    }

    OutputHandlerSTD std_output_handler_;
    OutputHandler   *output_handler_;
    OutputHandler   *previous_output_handler_;
    LogLevel         logLevel_;
    //bool                        logdata_;
    //bool                        endfile_;
    boost::mutex                lock_; // it is likely the outputhandler does some I/O, so we serialize it
};

// we use this function because we want to handle static initialization correctly
// however, the first run of this function is not thread safe, due to the use of a static
// variable inside the function. For this reason, we ensure the first call happens during
// static initialization using a proxy class
static DefaultOutputHandler* getDOH()
{
    static DefaultOutputHandler DOH;
    return &DOH;
}

#define USE_DOH                                                                \
    DefaultOutputHandler *doh = getDOH();                                      \
    boost::mutex::scoped_lock slock(doh->lock_)

#define MAX_BUFFER_SIZE 1024

/// @endcond
//static OutputHandler *STD = new OutputHandlerSTD();

void noOutputHandler()
{
    USE_DOH;
    doh->previous_output_handler_ = doh->output_handler_;
    doh->output_handler_ = NULL;
}

void restorePreviousOutputHandler()
{
    USE_DOH;
    std::swap(doh->previous_output_handler_, doh->output_handler_);
}

void useOutputHandler(OutputHandler *oh)
{
    USE_DOH;
    doh->previous_output_handler_ = doh->output_handler_;
    doh->output_handler_ = oh;
}

OutputHandler* getOutputHandler()
{
    return getDOH()->output_handler_;
}

void log3(OutputHandler *oh ,const bool endfile, const char *file,
        int line, LogLevel level, const char *m, ...)
{

    USE_DOH;
    doh->output_handler_ = oh;
    if (doh->output_handler_ && level >= doh->logLevel_)
    {
        va_list __ap;
        va_start(__ap, m);
        char buf[MAX_BUFFER_SIZE];


        vsnprintf(buf, sizeof(buf), m, __ap);
        va_end(__ap);
        buf[MAX_BUFFER_SIZE - 1] = '\0';
        //doh->output_handler_->logdata_ = doh->logdata_; // important !!
        doh->output_handler_->endfile_ = endfile;
        doh->output_handler_->log(buf, level, file,NULL, line);
    }
}

void log2(OutputHandler *oh , const char *file, int line,
        LogLevel level, const char *m, ...)
{

    USE_DOH;
    doh->output_handler_ = oh;
    if (doh->output_handler_ && level >= doh->logLevel_)
    {
        va_list __ap;
        va_start(__ap, m);
        char buf[MAX_BUFFER_SIZE];


        vsnprintf(buf, sizeof(buf), m, __ap);
        va_end(__ap);
        buf[MAX_BUFFER_SIZE - 1] = '\0';
        //doh->output_handler_->logdata_ = doh->logdata_; // important !!
        doh->output_handler_->log(buf, level, file, NULL, line);
    }
}


void log(const bool logonce,const char *file, const char* func,
        int line, LogLevel level, const char *m,  ...)
{
    USE_DOH;
    doh->output_handler_ = 
        static_cast<OutputHandler*>(&(doh->std_output_handler_));
    //useOutputHandler(STD);
    if (doh->output_handler_ && level >= doh->logLevel_)
    {
        va_list __ap;
        va_start(__ap, m);
        char buf[MAX_BUFFER_SIZE];


        vsnprintf(buf, sizeof(buf), m, __ap);
        va_end(__ap);
        buf[MAX_BUFFER_SIZE - 1] = '\0';
        doh->output_handler_->logonce_ = logonce; // important !!
        doh->output_handler_->log(buf, level, file, func , line);
    }
}

void setLogLevel(LogLevel level)
{
    USE_DOH;
    doh->logLevel_ = level;
}

LogLevel getLogLevel()
{
    USE_DOH;
    return doh->logLevel_;
}

/*void setDataLog(bool datalog)*/
//{
    //USE_DOH;
    ////wrong set here pointer is not static
    ////static_cast<OutputHandlerFile*>(doh->output_handler_)->datalog_ = datalog;
    //doh->logdata_ = datalog;
/*}*/
static const char *LogLevelString[5] = {"DEBUG:   ", "INFO:    ",
    "RUNNING: ", "WARNING: ", "ERROR:   "};

void OutputHandlerSTD::log(const std::string &text, LogLevel level,
        const char *filename, const char* func ,  int line)
{

    if(!onceflag_){
    if (level >= LOG_WARN)
    {
        switch (level)
        {
            case LOG_ERROR: printf(BLD_RED);
                break;
            case LOG_WARN: printf(BLD_YELLOW);
                break;
            default : printf(BLD_WHITE);
        };
        std::cerr << LogLevelString[level];
        printf(BLD_WHITE "[%s] " CLR_RESET, func);
        printf(CLR_WHITE);
        std::cout << text ;
#if ENABLE_LINE
        printf(BLD_BLUE"   [%d]" CLR_RESET, line);
#endif
        std::cout << std::endl;
        printf(BLD_YELLOW);
        std::cerr << " in " << filename << std::endl;
        printf(CLR_RESET);
        std::cerr.flush();
    }
    else
    {
        switch (level)
        {
            case LOG_RUN: printf(BLD_PURPLE);
                break;
            case LOG_INFO: printf(BLD_GREEN);
                break;
            case LOG_DEBUG: printf(BLD_WHITE);
                break;
            default : printf(BLD_WHITE);
        };
        std::cout << LogLevelString[level] ;
        printf(BLD_WHITE "[%s] " CLR_RESET, func);
        printf(CLR_WHITE);
        std::cout << text ;
#if ENABLE_LINE
        printf(BLD_BLUE"   [%d]" CLR_RESET, line);
#endif
        std::cout << std::endl;
        std::cout.flush();

        if(level == LOG_ERROR)
            std::getchar();

    }
    if(logonce_)
        onceflag_ = true;
    }
}

void OutputHandlerFile::constructHandlerFile(const char *filename ,
        const bool datalog, const bool oncelog) 
{
    //w+ means clean original 
    logdata_ = datalog;
    logonce_ = oncelog;
    file_ = fopen(filename, "w+");

    if (!file_)
        std::cerr << "Unable to open log file: '" << filename << "'" << std::endl;
}
OutputHandlerFile::OutputHandlerFile(const char *filename ,
        const bool datalog)
    : OutputHandler()
{
    constructHandlerFile(filename,datalog,false);
}

OutputHandlerFile::OutputHandlerFile(const char *filename ,
        const bool datalog, const bool oncelog) : OutputHandler()
{
    constructHandlerFile(filename,datalog,oncelog);
}

OutputHandlerFile::~OutputHandlerFile()
{
    if (file_)
        if (fclose(file_) != 0)
            std::cerr << "Error closing logfile" << std::endl;
}

void OutputHandlerFile::log(const std::string &text, LogLevel level,
        const char *filename, const char *func ,  int line)
{
    //printf(BLD_RED);
    if (file_ && !onceflag_)
    {
        if( logdata_ == true)
            if( endfile_ == true )
                fprintf(file_, "%s\n",  text.c_str());
            else
                fprintf(file_, "%s ",  text.c_str());
        else
            if( endfile_ == true )
                fprintf(file_, "%s%s\n", LogLevelString[level], text.c_str());
            else
                fprintf(file_, "%s%s ", LogLevelString[level], text.c_str());
        if(level >= LOG_WARN && logdata_ == false)
            fprintf(file_, "         at line %d in %s\n", line, filename);
        fflush(file_);

        if(logonce_)
            onceflag_ = true;
    }
}
