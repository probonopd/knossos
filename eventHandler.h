static uint32_t handleUserEvent(SDL_Event event, struct stateInfo *state);
static int32_t sdlToAgarEvent(void *obj, SDL_Event sdl_ev, AG_DriverEvent *ag_ev);
static uint32_t handleMouse(SDL_Event event);
static uint32_t handleMouseMotion(SDL_Event event, int32_t VPfound);
static uint32_t handleMouseMotionLeftHold(SDL_Event event, int32_t VPfound);
static uint32_t handleMouseMotionMiddleHold(SDL_Event event, int32_t VPfound);
static uint32_t handleMouseMotionRightHold(SDL_Event event, int32_t VPfound);
static uint32_t handleMouseButtonLeft(SDL_Event event, int32_t VPfound);
static uint32_t handleMouseButtonMiddle(SDL_Event event, int32_t VPfound);
static uint32_t handleMouseButtonRight(SDL_Event event, int32_t VPfound);
static uint32_t handleMouseButtonWheelForward(SDL_Event event, int32_t VPfound);
static uint32_t handleMouseButtonWheelBackward(SDL_Event event, int32_t VPfound);
static uint32_t handleKeyboard(SDL_Event event);
static Coordinate *getCoordinateFromOrthogonalClick(int32_t coordinateMag, SDL_Event event, int32_t VPfound);
static int32_t checkForViewPortWdgt(AG_Widget *wdgt);
