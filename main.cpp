#include <iostream>
#include <memory>
#include "HSMP.hpp"

int main() {
  std::shared_ptr<HSMP::ClientRequest> req = HSMP::ProcessRequest("l1104santistebanucsp");
  req->PrintStructure();

  req = HSMP::ProcessRequest("i");
  req->PrintStructure();

  req = HSMP::ProcessRequest("m00505hellomateo");
  req->PrintStructure();

  req = HSMP::ProcessRequest("f05mateo");
  req->PrintStructure();

  req = HSMP::ProcessRequest("x");
  req->PrintStructure();

  return 0;
}
