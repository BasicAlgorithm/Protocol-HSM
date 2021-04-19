#include <iostream>
#include <memory>
#include "HSMP.hpp"

int main() {
  std::shared_ptr<HSMP::ClientRequest> req;

  req=  HSMP::ProcessRequest("l1104santistebanucsp");
  req->PrintStructure();

  req = HSMP::ProcessRequest("i");
  req->PrintStructure();

  req = HSMP::ProcessRequest("m00505hellomateo");
  req->PrintStructure();

  req = HSMP::ProcessRequest("f05mateo");
  req->PrintStructure();

  req = HSMP::ProcessRequest("x");
  req->PrintStructure();

  std::shared_ptr<HSMP::ServerResponse> res;

  res = HSMP::ProcessResponse("Lok");
  res->PrintStructure();

  res = HSMP::ProcessResponse("I03110305SantistebanLeePeter");
  res->PrintStructure();

  res = HSMP::ProcessResponse("M00305byemateo");
  res->PrintStructure();

  res = HSMP::ProcessResponse(""); //Broadcast
  // res->PrintStructure();
  
  res = HSMP::ProcessResponse(""); // Upload
  // res->PrintStructure();

  res = HSMP::ProcessResponse("F05mateo");
  res->PrintStructure();

  res = HSMP::ProcessResponse("X");
  res->PrintStructure();

  res = HSMP::ProcessResponse("EThis is a Error Message");
  res->PrintStructure();

  return 0;
}
