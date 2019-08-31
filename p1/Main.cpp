// Author(s): Paulo Pagliosa, Yago Mescolotte, Carlos Monteiro
// Last revision: 31/08/2019

#include "P1.h"

int
main(int argc, char** argv)
{
  return cg::Application{new P1{1280, 720}}.run(argc, argv);
}
