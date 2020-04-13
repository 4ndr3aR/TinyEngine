//Data
const int SIZE = 100;
int data[SIZE*SIZE] = {0};
bool paused = true;

//Setup of Data
void setup(){
  //Generate Random Data!
  srand(time(NULL));
  for(int i = 0; i < SIZE*SIZE; i++){
    data[i] = (rand()%2 == 0)?1:0;
  }
};

// Event Handler
std::function<void()> eventHandler = [&](){

  // Pause and Unpause
  if(!Tiny::event.keys.empty()){
    if(Tiny::event.keys.back().key.keysym.sym == SDLK_p){
      paused = !paused;
    }
  }

};