
// Standard library:
#include <iostream>

//
#include "rabbitmq/parameters.h"
#include "rabbitmq/connection.h"
#include "rabbitmq/channel.h"

void receive_logs_direct (const int nb_severties_, const char** severities_);

int main (const int argc, const char** argv)
{
  int error_code = EXIT_SUCCESS;
  try {
    if (argc > 1) {
       receive_logs_direct (argc, argv);
    } else {
       std::cerr << "usage : " << argv [0] << " [info] [warning] [error]" << std::endl;
       return EXIT_FAILURE;
    }
  } catch (std::exception & x) {
    std::cerr << "ERROR: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "ERROR: " << "Unexpected exception!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return error_code;
}

void my_callback (std::string & msg_, std::string & rkey_, rabbitmq::basic_properties & props_) {
   std::clog << " [x]  " << rkey_ << "  " << msg_ << std::endl;
}

void receive_logs_direct (const int nb_severities_, const char** severities_)
{
   std::clog << "\nTUTORIAL 4 : 'Routing' - receive logs direct\n\n" ;
   rabbitmq::connection_parameters c_par;
   rabbitmq::connection            con (c_par);
   if (con.is_ok ()) {
      rabbitmq::channel &           chan = con.grab_channel ();
      rabbitmq::exchange_parameters x_par;
      rabbitmq::queue_parameters    q_par;
      x_par.name      = "direct_logs";
      x_par.type      = "direct";
      q_par.name      = "";                                          // q_par = "" -> rabbit will choose one for us
      q_par.exclusive = true;
      chan.exchange_declare (x_par);
      chan.queue_declare    (q_par);
      for (int s=0; s<nb_severities_; s++) {
         chan.queue_bind (q_par.name, x_par.name, severities_ [s]);  // q_par.name <- choosen by rabbit
      }
      chan.basic_consume   (q_par.name, "", true);    // no_ack = true
      std::clog << " [*] Waiting for logs. Press Return to exit." << std::endl;
      chan.start_consuming (&my_callback);
      std::cin.ignore      ();
      chan.stop_consuming  ();
   } else {
      std::cerr << "fail to connect ..." << std::endl;
   }
   return;
}



