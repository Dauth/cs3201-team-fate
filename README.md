# cs3201-team-fate

Iteration 2

Expected Design Changes:

  PKB:
    API calls for design abstractions will return pair values 
    Design extractor will be encapsulated within PKB
    Polymorphism for node objects
      abstract method interfaces to make value retrieval of object calls independent of object type
      eg. getValue() for variables, constants etc instead of ->getValue() and ->getVariable()->getName()
    Follows design abstraction will use a linked list
    CFG will use a graph
      Recursive search for next and affects path
      Queue object for tracking path through containers
    
    
