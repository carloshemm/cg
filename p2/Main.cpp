// Author(s): Paulo Pagliosa, Carlos Monteiro e Yago Mescolotte
// Last revision: 15/10/2019

#include "P2.h"

int
main(int argc, char** argv)
{
  return cg::Application{new P2{1280, 720}}.run(argc, argv);
}
