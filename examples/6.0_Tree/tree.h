#include <TinyEngine/color>

#include <random>

#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

void init_logging()
{
   boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::info); 
} 

struct Branch{

  int ID = 0;         //For Leaf Hashing
  bool leaf = true;

  Branch *A, *B, *P;  //Child A, B and Parent

  //Parameters
  float ratio, spread, splitsize;
  int depth = 0;

  //Constructors
  Branch(float r, float s, float ss):
    ratio       {r},
    spread      {s},
    splitsize   {ss}
  {};

  Branch(Branch* b, bool root):
    ratio       {b->ratio},
    spread      {b->spread},
    splitsize   {b->splitsize}
  {
    if(root) return;
    depth = b->depth+1;
    P = b;  //Set Parent
  };

  ~Branch(){
    if(leaf) return;
    delete(A);
    delete(B);
  }

  //Size / Direction Data
  glm::vec3 dir = glm::vec3(0.0, 1.0, 0.0);
  float length = 0.0, radius = 0.0, area = 0.1;

  void grow(double _size);
  void split();

  //Compute Direction to Highest Local Leaf Density
  glm::vec3 leafdensity(int searchdepth);
};

void Branch::grow(double feed){

  radius = sqrt(area/PI);   //Current Radius

  if(leaf){
    length += cbrt(feed);   //Grow in Length
    feed -= cbrt(feed) * area;  //Reduce Feed
    area += feed/length;    //Grow In Area

    //Split Condition
    if(length > splitsize * exp(-splitdecay * depth))
      split();  //Split Behavior

    return;
  }

  double pass = passratio;

  if(conservearea)  //Feedback Control for Area Conservation
    pass = (A->area+B->area)/(A->area+B->area+area);

  area += pass * feed / length;   //Grow in Girth
  feed *= ( 1.0 - pass );         //Reduce Feed

  if(feed < 1E-5) return;         //Prevent Over-Branching

  A->grow(feed*ratio);            //Grow Children
  B->grow(feed*(1.0-ratio));
}

void Branch::split(){

  leaf = false;

  //Add Child Branches
  A = new Branch(this, false);
  B = new Branch(this, false);
  A->ID = 2 * ID + 0; //Every Leaf ID is Unique (because binary!)
  B->ID = 2 * ID + 1;

  /*  Ideal Growth Direction:
        Perpendicular to direction with highest leaf density! */

  glm::vec3 D = leafdensity(localdepth);            //Direction of Highest Density
  glm::vec3 N = glm::normalize(glm::cross(dir, D)); //Normal Vector
  glm::vec3 M = -1.0f*N;                            //Reflection

  float flip = (rand()%2)?1.0:-1.0; //Random Direction Flip
  A->dir = glm::normalize( glm::mix(flip*spread*N, dir*fastigiate,     ratio) );
  B->dir = glm::normalize( glm::mix(flip*spread*M, dir*fastigiate, 1.0-ratio) );

}

glm::vec3 Branch::leafdensity(int searchdepth){

  //Random Vector! (for noise)
  glm::vec3 r = glm::vec3(rand()%100,rand()%100,rand()%100)/glm::vec3(100)-glm::vec3(0.5);

  if(depth == 0) return r;

  /*
    General Idea: Branches grow away from areas with a high leaf density!

    Therefore, if we can compute a vector that points towards the area with
    the locally highest leaf density, we can use that to compute our normal
    for branching.

    Locally high density is determined by descending the tree to some maximum
    search depth (finding an ancestor node), and computing some leaf-density
    metric over the descendant node leaves. This is implemented recursively.

    Metric 1: Uniform Weights in Space.
      Problem: Causes strange spiral artifacts at high-search depths, because it
        computes the average leaf position of the entire tree. Therefore,
        the tree grows in a strange way, away from the center.

    Metric 2: Distance weighted average position (i.e. relative vector)
      Problem: This causes strange cone artifacts when using a sufficiently large
        search depth. This is also more expensive to compute, and wonky because
        we compute the distance-weighted average distance (what?? exactly).

    Since both metrics give similar results at a small search-depth (e.g. 2),
    meaning we only search locally, I will use the one that is simpler to compute.
    That is Method 1.

    I did throw in a weighting by the branch ratio though, just because I can.
    That means that the tree should tend to grow away from branches with the most
    growth potential.

  */

  Branch* C = this;                                 //Ancestor node
  glm::vec3 rel = glm::vec3(0);                     //Relative position to start node
  while(C->depth > 0 && searchdepth-- >= 0){        //Descend tree
    rel += C->length*C->dir;                        //Add relative position
    C = C->P;                                       //Move to parent
  }

  std::function<glm::vec3(Branch*)> leafaverage = [&](Branch* b)->glm::vec3{
    if(b->leaf) return b->length*b->dir;
    return b->length*b->dir + ratio*leafaverage(b->A) + (1.0f-ratio)*leafaverage(b->B);
  };

  //Average relative to ancestor, shifted by rel ( + Noise )
  return directedness*glm::normalize(leafaverage(C) - rel) + (1.0f-directedness)*r;
}

Branch* root;


void construct(Buffer& positions, Buffer& normals, Buffer& colors, Buffer& indices){

  std::vector<int> indbuf;
  std::vector<float> posbuf, norbuf, colbuf;

  //Basically Add Lines for the Tree!
  std::function<void(Branch*, glm::vec3)> addBranch = [&](Branch* b, glm::vec3 p){

    glm::vec3 start = p;
    glm::vec3 end   = p + glm::vec3(b->length*treescale[0])*b->dir;

    //Get Some Normal Vector
    glm::vec3 x = glm::normalize(b->dir + glm::vec3(1.0, 1.0, 1.0));
    glm::vec4 n = glm::vec4(glm::normalize(glm::cross(b->dir, x)), 1.0);

    //Add the Correct Number of Indices
    glm::mat4 r = glm::rotate(glm::mat4(1.0), PI/ringsize, b->dir);

    //Index Buffer
    int _b = posbuf.size()/3;

    //GL TRIANGLES
    for(int i = 0; i < ringsize; i++){
      //Bottom Triangle
      indbuf.push_back(_b+i*2+0);
      indbuf.push_back(_b+(i*2+2)%(2*ringsize));
      indbuf.push_back(_b+i*2+1);
      //Upper Triangle
      indbuf.push_back(_b+(i*2+2)%(2*ringsize));
      indbuf.push_back(_b+(i*2+3)%(2*ringsize));
      indbuf.push_back(_b+i*2+1);
    }

    for(int i = 0; i < ringsize; i++){

      posbuf.push_back(start.x + b->radius*treescale[1]*n.x);
      posbuf.push_back(start.y + b->radius*treescale[1]*n.y);
      posbuf.push_back(start.z + b->radius*treescale[1]*n.z);
      norbuf.push_back(n.x);
      norbuf.push_back(n.y);
      norbuf.push_back(n.z);
      n = r*n;

      posbuf.push_back(end.x + taper*b->radius*treescale[1]*n.x);
      posbuf.push_back(end.y + taper*b->radius*treescale[1]*n.y);
      posbuf.push_back(end.z + taper*b->radius*treescale[1]*n.z);
      norbuf.push_back(n.x);
      norbuf.push_back(n.y);
      norbuf.push_back(n.z);
      n = r*n;

    }

    //No Leaves
    if(b->leaf) return;

    addBranch(b->A, end);
    addBranch(b->B, end);
  };

  //Recursive add Branches
  addBranch(root, glm::vec3(0.0));

  positions.fill<float>(posbuf);
  normals.fill<float>(norbuf);
  colors.fill<float>(colbuf);
  indices.fill<int>(indbuf);

};

//Construct Leaf Particle System from Tree Data
std::function<void(std::vector<glm::mat4>&, std::vector<glm::uvec3>&, bool)> addLeaves = [](std::vector<glm::mat4>& p, std::vector<glm::uvec3>& a, bool face){
  BOOST_LOG_TRIVIAL(trace) << "Entering addLeaves()..." << std::endl;
  p.clear();

  std::random_device rd;                  // Will be used to obtain a seed for the random number engine
  std::mt19937 gen(rd());                 // Standard mersenne_twister_engine seeded with rd()
  std::uniform_int_distribution<> leaf_angle_rng(0, 45);

  //bool gen_angles = (a.size() == 0);      // Generate leaf u,v,w angles only once (if this vector is empty)

  //Explore the Tree and Add Leaves!
  std::function<void(Branch*, glm::vec3)> addLeaf = [&](Branch* b, glm::vec3 pos)
  {
    BOOST_LOG_TRIVIAL(trace) << "Entering addLeaf()..." << std::endl;

    if (b->leaf)
    {

      if (b->depth < leafmindepth)
      {
        BOOST_LOG_TRIVIAL(trace) << "Departing addLeaf() very soon..." << std::endl;
        return;
      }

      for(int i = 0; i < leafcount; i++){
        //Hashed Random Displace
        glm::vec3 d = glm::vec3(color::hashrand(b->ID+i), color::hashrand(b->ID+i+leafcount), color::hashrand(b->ID+i+2*leafcount))-glm::vec3(0.5);
        d = d * glm::vec3(leafspread[0], leafspread[1], leafspread[2]);

        glm::mat4 model = glm::translate(glm::mat4(1.0), pos+d);

        if (a.size() == 0 || a.size()-1 < p.size())                                     // Generate leaf u,v,w angles only if we don't have already generated them
        {
                BOOST_LOG_TRIVIAL(debug) << "GEN len(p) = " << p.size() << " - len(a) = " << a.size() << std::endl;
                uint8_t u = leaf_angle_rng(gen);
                uint8_t v = leaf_angle_rng(gen);
                uint8_t w = leaf_angle_rng(gen);
                glm::uvec3 angles = glm::uvec3(u, v, w);
                a.push_back(angles);
                BOOST_LOG_TRIVIAL(debug) << "GEN adding angle: " << +u << " - " << +v << " - " << +w << std::endl;
        }
        else
                BOOST_LOG_TRIVIAL(debug) << "NOGEN len(p) = " << p.size() << " - len(a) = " << a.size() << std::endl;

        auto ang = a[p.size()];
        BOOST_LOG_TRIVIAL(debug) << "BOTH ang = " << ang.x << " - " << ang.y << " - " << ang.z << " --- len(p) = " << p.size() << " - len(a) = " << a.size() << std::endl;

        model = glm::rotate(model, float(ang.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, float(ang.y), glm::vec3(0.0f, 1.0f, 0.0f)); 
        model = glm::rotate(model, float(ang.z), glm::vec3(0.0f, 0.0f, 1.0f)); 

        p.push_back(glm::scale(model, glm::vec3(leafsize)));

      }

      return;
    }

    //Otherwise get other leaves!
    glm::vec3 end   = pos + glm::vec3(b->length*treescale[0])*b->dir;
    addLeaf(b->A, end);
    addLeaf(b->B, end);
  };

  addLeaf(root, glm::vec3(0.0));
  /*
  if (p.size() > 100) 
        Tiny::quit(); 
  */
};
