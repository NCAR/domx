

#include "Car.h"
#include "Repairs.h"

#include "domx/XmlObjectCatalog.h"
#include "domx/XmlTime.h"
#include "domx/XmlFileObject.h"

#include <logx/Logging.h>

LOGGING("domx-tests");

#include <iostream>
#include <sstream>
#include <time.h>

using namespace domx;
using std::endl;
using std::cerr;


#define Check(cond) \
do { if (!(cond)) { \
std::cerr << "Check failed at " << __FILE__ << ":" << __LINE__ \
<< ": " << #cond << "\n"; \
++errors; }} while (0)


void
make_mazda(Car& c)
{
  c.setAxles(2);
  c.setMake("mazda");
  c.setModel ("323");
  c.Color = "white";
  c.Year = 1986;
}


void
make_honda(Car& c)
{
  c.setMake("honda");
  c.setModel ("odyssey");
  c.setSpeed(25);
  c.setAxles(2);
  c.Color = "brightwhite";
  c.Year = 2002;
}


int
compare_honda(Car& van)
{
  int errors = 0;
  Check(van.getMake() == "honda");
  Check(van.getModel() == "odyssey");
  Check(van.getSpeed() == 25);
  Check(van.getAxles() == 2);
  Check(van.Color() == "brightwhite");
  Check(van.Year() == 2002);
  return errors;
}


int
test_xmlobject ()
{
  int errors = 0;

  // Create a Vehicle and start extending it.
  Vehicle v;

  Check(v.getSpeed() == 0);
  Check(v.getAxles() == 0);

  // Create a Car and assume a vehicle into it.
  Car c;
  c.assume(v);
  Check(c.interfaceName() == "xmlobject.vehicle.car");

  Check (c.getSpeed() == 0);
  Check (c.getAxles() == 0);
  Check (c.getMake() == "");
  Check (c.getModel() == "");
  Check (c.Color.get() == "white");
  Check (c.Year.get() == 1986);

  // Now modify the vehicle.
  v.setSpeed(64);
  v.setAxles(2);

  // Make sure the vehicle changed but the car didn't.
  Check (c.getSpeed() == 0);
  Check (c.getAxles() == 0);
  Check(v.getSpeed() == 64);
  Check(v.getAxles() == 2);

  // See if we can change the car.
  make_mazda (c);
  c.Color = "dirty";
  Check (c.getMake() == "mazda");
  Check (c.getModel() == "323");
  Check (c.Color.get() == "dirty");

  // Now see if the car changes when reassigned with the vehicle.
  // The car interface should be reset to the defaults.
  c.assume(v);
  Check (c.getMake() == "");
  Check (c.getModel() == "");
  Check(c.getSpeed() == 64);
  Check(c.getAxles() == 2);
  Check (c.Year() == 1986);
  Check (c.Color() == "white");

  // Check that we can assign a Car to a Vehicle and back without
  // losing the information in the Car interface.
  make_honda(c);
  c.setSpeed(32);
  v = c;
  Check(v.getSpeed() == 32);
  Check(v.getAxles() == 2);

  Car van;
  van.assume (v);
  Check(van.getMake() == "honda");
  Check(van.getModel() == "odyssey");
  Check(van.getSpeed() == 32);
  Check(van.getAxles() == 2);
  Check(van.Color() == "brightwhite");
  Check(van.Year() == 2002);

  // Store van to a file.
  Check(van.store ("van.xml"));

  // Reset van.
  Car empty;
  DLOG << "default Car:\n";
  DLOG << empty.toString ();
  van.assume (empty);
  Check(van.getMake() != "honda");
  Check(van.getModel() != "odyssey");

  // Compare again.
  van.load("van.xml");
  Check(van.getMake() == "honda");
  Check(van.getModel() == "odyssey");
  Check(van.getSpeed() == 32);
  Check(van.getAxles() == 2);
  Check(van.Color() == "brightwhite");
  Check(van.Year() == 2002);

  DLOG << "vehicle: \n";
  DLOG << v.toString ();
  DLOG << "car: \n";
  DLOG << c.toString ();
  DLOG << "van: \n";
  DLOG << van.toString ();

  // See what happens with a broken load.
  Repairs r;
  r.setSparkGap (0.2);
  Check (! r.fromXML(""));
  Check (r.getSparkGap() != 0);

  // Check the time member of a Repair object associated with the van.
  r.assume (van);
  r.NextVisit = 1276041409L;
  Check(r.NextVisit().key() == "20100608.235649");
  Check(r.NextVisit().toString() == "2010-06-08 23:56:49");
  Check(r.store ("vanrepairs.xml"));
  r.assume(Repairs());
  Check(r.NextVisit() == 0);
  Check(r.load ("vanrepairs.xml"));
  Check(r.NextVisit().key() == "20100608.235649");
  Check(r.NextVisit().toString() == "2010-06-08 23:56:49");

  // Now check that we can add an arbitrary interface to an
  // arbitrary object.
  //
  XmlObjectInterface any;
  Check(any.interfaceName() == "xmlobject");
  Check(any.getInterface("xmlobject") == &any);
  Check(any.getInterface("vehicle") == 0);
  Vehicle* vp = any.getInterface<Vehicle>(false);
  Check(vp == 0);
  vp = any.getInterface<Vehicle>(true);
  Check(vp != 0);
  if (vp)
  {
    vp->setSpeed(75);
    Check (vp->getSpeed() == 75);
    DLOG << vp->interfaceName();
    DLOG << vp->toString();
  }

  return errors;
}


int
test_xmlobjectcatalog()
{
  int errors = 0;

  // Create a catalog.
  XmlObjectCatalog vehicles;
  domx::XmlObjectCatalog::setRootCatalogDirectory(".");
  Check (vehicles.open ("family-cars"));

  Car c;
  make_mazda(c);
  Check(vehicles.insert("mazda", &c));
  make_honda(c);
  Check(vehicles.insert("honda", &c));

  XmlObjectCatalog::key_set_t keys;
  Check(vehicles.keys(keys));
  Check(keys.size() == 2);
  if (keys.size() == 2)
  {
    XmlObjectCatalog::key_set_t::iterator it = keys.begin();
    Check(*it == "honda");
    Check(*(++it) == "mazda");
  }
  // Load it as a generic object, assume it into a Car, and
  // see if everything is still there.
  XmlObjectInterface xo;
  Check(vehicles.load("honda", &xo));
  Car honda;
  honda.assume (xo);
  errors += compare_honda(honda);

  Check(vehicles.remove ("honda"));
  Check(vehicles.keys(keys));
  Check(keys.size() == 1);
  Check(! vehicles.load("honda", &xo));

  return errors;
}


int
test_xmltime()
{
  int errors = 0;

  XmlTime now = time(0);
  std::ostringstream out;
  out << now;
  ILOG << out.str();
  XmlTime then;
  Check (then != now);
  std::istringstream in(out.str());
  in >> then;
  ILOG << then.toString();
  Check (then == now);
  then = 0;
  then.fromString(out.str());
  Check (then == now);

  // Check key().
  std::string bday("2003-07-13 12:34:56");
  std::istringstream bin (bday);
  bin >> then;
  ILOG << bday << " has key(): " << then.key();
  Check(then.key() == "20030713.123456");
  return errors;
}


int
test_xmlfileobject()
{
  int errors = 0;

  // Create

  return errors;
}



int
test_xmlstring()
{
  int errors = 0;
  // Make sure that an xml string member preserves spaces.
  XmlFileObject xfo;

  std::string path = "this string has spaces";
  xfo.Directory = path;
  Check(xfo.Directory() == path);
  return errors;
}


int 
main(int argc, char* argv[])
{
  logx::ParseLogArgs (argc, argv);
  try 
  {
    int errors = 0;
    errors += test_xmlobject();
    errors += test_xmlobjectcatalog();
    errors += test_xmltime();
    errors += test_xmlfileobject();
    errors += test_xmlstring();

    if (errors == 0)
    {
      cerr << "No errors.\n";
    }
    else
    {
      cerr << errors << " errors.\n";
    }
  }
  catch (const XMLException& e)
  {
    cerr << "XMLException: " << xstring(e.getMessage()) << endl;
  }
  catch (const xercesc::DOMException& e)
  {
    cerr << "DOMException(" << e.code << "): " << xstring(e.msg) << endl;
  }
  catch (...)
  {
    cerr << "unknown exception" << endl;
  }
  return 1;
}

