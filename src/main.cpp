//Need to load screensht images to IMGUI windows
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//IMGUI libraries to support SDL2 rendering
#include "imgui.h"
#include "imgui_markdown.h" 
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "keys.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <fstream>
#include <vector>
#include <sstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <argp.h>
#include <fstream>


#if !SDL_VERSION_ATLEAST(2,0,17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif


// need to mention a version string.
const char *argp_program_version = "Portmaster Game Options 0.0.12";

// documentation string that will be displayed in the help section.
static char doc[] = "This is an onging deveopment. Please feedback suggestions and changes via the github page. https://github.com/monkeyx-net/Imgui4Portmaster";

// email address for bug reporting.
const char *argp_program_bug_address = "tim@monkeyx.net";

// argument list for doc. This will be displayed on --help
static char args_doc[] = "-w Screen width, -p players etc";

// cli argument availble options.
static struct argp_option options[] = {
    {"verbose", 'v', 0, 0, "Produce verbose output"},
    {"width", 'w', "Width", 0,"Screen Width"},
    {"height", 'h', "Height", 0,"Screen Height"},
    {"scale", 's', "Scale", 0,"Screen Scale"},
    {"players", 'p', "Players", 0,"Number of players 0 - 4. 0 players prevents showing the IP Dialogue"},
    {"title", 't', "Game", 0,"Game Title"},
    {0}
};

// define a struct to hold the arguments.
struct arguments{
    int  verbose;
    char *args[1];
    char *width;
    char *height;
    char *scale;
    char *players;
    char *title;
};

// define a function which will parse the args.
static error_t parse_opt(int key, char *arg, struct argp_state *state){

    struct arguments *arguments = (struct arguments*)state->input;
    switch(key){

        case 'v':
            arguments->verbose = 1;
            break;
        case 'w':
            arguments->width = arg;
            break;
        case 'h':
            arguments->height = arg;
            break;
        case 's':
            arguments->scale = arg;
            break;
         case 'p':
            arguments->players = arg;
            break;
         case 't':
            arguments->title = arg;
            break;
        case ARGP_KEY_ARG: 
            // Too many arguments.
            if(state->arg_num > 0)
                argp_usage(state);
            arguments->args[state->arg_num] = arg;
            break;

        case ARGP_KEY_END:
            // Not enough arguments.
            if(state->arg_num < 0)
                argp_usage(state);
            break;

        default:
            return ARGP_ERR_UNKNOWN;
    }

    return 0;
}

// initialize the argp struct. Which will be used to parse and use the args.
static struct argp argp = {options, parse_opt, args_doc, doc};

inline ImGui::MarkdownImageData ImageCallback( ImGui::MarkdownLinkCallbackData data_ );

static ImFont* H1 = NULL;
static ImFont* H2 = NULL;
static ImFont* H3 = NULL;

static ImGui::MarkdownConfig mdConfig;  

inline ImGui::MarkdownImageData ImageCallback( ImGui::MarkdownLinkCallbackData data_ )
{
    // In your application you would load an image based on data_ input. Here we just use the imgui font texture.
    ImTextureID image = ImGui::GetIO().Fonts->TexID;
    // > C++14 can use ImGui::MarkdownImageData imageData{ true, false, image, ImVec2( 40.0f, 20.0f ) };
    ImGui::MarkdownImageData imageData;
    imageData.isValid =         true;
    imageData.useLinkCallback = false;
    imageData.user_texture_id = image;
    imageData.size =            ImVec2( 40.0f, 20.0f );

    // For image resize when available size.x > image width, add
    ImVec2 const contentSize = ImGui::GetContentRegionAvail();
    if( imageData.size.x > contentSize.x )
    {
        float const ratio = imageData.size.y/imageData.size.x;
        imageData.size.x = contentSize.x;
        imageData.size.y = contentSize.x*ratio;
    }

    return imageData;
}

void LoadFonts( float fontSize_ = 12.0f )
{
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Clear();
    // Base font
    io.Fonts->AddFontFromFileTTF( "Assets/Fonts/Roboto-Medium.ttf", fontSize_ );
    // Bold headings H2 and H3
    H2 = io.Fonts->AddFontFromFileTTF( "Assets/Fonts/Roboto-Medium.ttf", fontSize_ );
    H3 = mdConfig.headingFormats[ 1 ].font;
    // bold heading H1
    float fontSizeH1 = fontSize_ * 1.1f;
    H1 = io.Fonts->AddFontFromFileTTF( "Assets/Fonts/Roboto-Medium.ttf", fontSizeH1 );
}

void ExampleMarkdownFormatCallback( const ImGui::MarkdownFormatInfo& markdownFormatInfo_, bool start_ )
{
    // Call the default first so any settings can be overwritten by our implementation.
    // Alternatively could be called or not called in a switch statement on a case by case basis.
    // See defaultMarkdownFormatCallback definition for furhter examples of how to use it.
    ImGui::defaultMarkdownFormatCallback( markdownFormatInfo_, start_ );        
       
    switch( markdownFormatInfo_.type )
    {
    // example: change the colour of heading level 2
    case ImGui::MarkdownFormatType::HEADING:
    {
        if( markdownFormatInfo_.level == 2 )
        {
            if( start_ )
            {
                ImGui::PushStyleColor( ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled] );
            }
            else
            {
                ImGui::PopStyleColor();
            }
        }
        break;
    }
    default:
    {
        break;
    }
    }
}

void Markdown( const std::string& markdown_ )
{
    // You can make your own Markdown function with your prefered string container and markdown config.
    // > C++14 can use ImGui::MarkdownConfig mdConfig{ LinkCallback, NULL, ImageCallback, ICON_FA_LINK, { { H1, true }, { H2, true }, { H3, false } }, NULL };
    mdConfig.linkCallback =         NULL;
    mdConfig.tooltipCallback =      NULL;
    mdConfig.imageCallback =        ImageCallback;
    mdConfig.linkIcon =             NULL;
    mdConfig.headingFormats[0] =    { H1, true };
    mdConfig.headingFormats[1] =    { H2, true };
    mdConfig.headingFormats[2] =    { H3, false };
    mdConfig.userData =             NULL;
    mdConfig.formatCallback =       ExampleMarkdownFormatCallback;
    ImGui::Markdown( markdown_.c_str(), markdown_.length(), mdConfig );
}

//inline bool file_exists (const std::string& name) {
std::string file_exists (std::string fname) 
{
    if (FILE *file = fopen(fname.c_str(), "r")) {
        fclose(file);
    }
    else{
        fname = "";
    }
    return fname;
}

std::string read_file(std::ifstream in_text )
{
    //std::ifstream in_text (in_text);
    std::string line;
    std::string all_lines;
    if (in_text.is_open())
    {
        while ( getline (in_text,line) )
        {
            all_lines += line + "\n";
        }
    in_text.close();
  }
  return all_lines;
}

std::string write_file(std::string filename, int player, int vecIp4[])
{
    //printf("Player = %d.%d.%d.%d", vecIp4[0],vecIp4[1],vecIp4[2],vecIp4[3]);
    std::ostringstream stream;
    for (int i = 0; i < 4; i++)
    {
        if (i) stream << '.';
        stream << vecIp4[i];
    }
    std::ofstream file(filename);
    std::string ip_string = stream.str();
    file << ip_string;
    file.close();
    //ips_load = ip_string;
    return ip_string;
}

// Run system command and capture output
std::string exec(const char* cmd) {
    char buffer[128];
    std::string result = "";
    FILE* pipe = popen(cmd, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (fgets(buffer, sizeof buffer, pipe) != NULL) {
            result += buffer;
        }
    } catch (...) {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    return result;
}


// Load texture for IMGUI
bool LoadTextureFromFile(const char* filename, SDL_Texture** texture_ptr, int& width, int& height, SDL_Renderer* bg_renderer) 
{
    int channels;
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 0);

    if (data == nullptr) {
        fprintf(stderr, "Failed to load image: %s\n", stbi_failure_reason());
        return false;
    }

    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom((void*)data, width, height, channels * 8, channels * width,
                                                    0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);

    if (surface == nullptr) {
        fprintf(stderr, "Failed to create SDL surface: %s\n", SDL_GetError());
        return false;
    }

    *texture_ptr = SDL_CreateTextureFromSurface(bg_renderer, surface);

    if ((*texture_ptr) == nullptr) {
        fprintf(stderr, "Failed to create SDL texture: %s\n", SDL_GetError());
    }

    SDL_FreeSurface(surface);
    stbi_image_free(data);

    return true;
}

std::vector<std::string> split (const std::string &s, char delim)
 {
    std::vector<std::string> result;

    std::stringstream ss (s);
    std::string item;
    while (getline (ss, item, delim)) {
        result.push_back (item);
    }

    return result;
}

// Main code
int main(int argc, char *args[])
{
        //Read ip and instructions files before the starting the loop
    std::string ips_load=read_file(std::ifstream("ip_server.txt"));
    std::string ip1_load=read_file(std::ifstream("ip1.txt"));
    std::string ip2_load=read_file(std::ifstream("ip2.txt"));
    std::string ip3_load=read_file(std::ifstream("ip3.txt"));
    std::string ip4_load=read_file(std::ifstream("ip4.txt"));
    std::string instructions=read_file(std::ifstream("instructions.txt"));
    std::string readme=read_file(std::ifstream("README.md"));
    std::string licence=read_file(std::ifstream("imgui-demo_LICENSE.txt"));
    std::string file_debug = exec("file imgui-demo");
    std::string ldd_debug = exec("ldd imgui-demo");
    std::string log_debug=read_file(std::ifstream("log.txt"));
    // Vector for IP Address Input  ImGui::DragInt4
    static int vec4i_svr1[3];// = { 127, 0, 0, 1 };
    static int vec4i_ply1[3];
    static int vec4i_ply2[3];
    static int vec4i_ply3[3];
    static int vec4i_ply4[3]; 
    static int iplayer = 4;
    static int mplayer = 4;
    int selected = 0;
    //bool full_debug = false; 
    bool done = false;
    bool show_ip_window = false;
    bool game_sshot = false;
    Mix_Chunk* gHigh = NULL;


    std::string input;
    // create a new struct to hold arguments.
    struct arguments arguments;

    // set the default values for all of the args.
    arguments.verbose = 0;
    arguments.width = NULL;
    arguments.height = NULL;
    arguments.scale = NULL;
    arguments.players = NULL;
    arguments.title = NULL;

    // parse the cli arguments.
    argp_parse(&argp, argc, args, 0, 0, &arguments);


    // Do as function to process args?
    if (arguments.players)
    {
     int ptemp = atoi(arguments.players);
     if (ptemp >0 && ptemp <5)
        {
         //std::cout << "Players " << players;
         mplayer=ptemp;
         iplayer=ptemp;
        }
    else
        {
            printf("Wrong number of players selected: %d\n Select between 0 and 4", ptemp);
            return -1;
        }    
    }
    if (arguments.title)
    {

    }

    SDL_Texture *my_texture = NULL;
    //SDL_Renderer *renderer = NULL;

    //const ImU8  u8_min = 1, u8_max = 4;
    
    // Make this a function!
    std::vector<std::string> v1 = split (ips_load, '.');
    for (int i = 0; i < 4; i++)
    {
        vec4i_svr1[i] = atoi(v1[i].c_str());
    }
    std::vector<std::string> v2 = split (ip1_load, '.');
    for (int i = 0; i < 4; i++)
    {
        vec4i_ply1[i] = atoi(v2[i].c_str());
    }
    std::vector<std::string> v3 = split (ip2_load, '.');
    for (int i = 0; i < 4; i++)
    {
        vec4i_ply2[i] = atoi(v3[i].c_str());
    }
    std::vector<std::string> v4 = split (ip3_load, '.');
    for (int i = 0; i < 4; i++)
    {
        vec4i_ply3[i] = atoi(v4[i].c_str());
    }
    std::vector<std::string> v5 = split (ip4_load, '.');
    for (int i = 0; i < 4; i++)
    {
        vec4i_ply4[i] = atoi(v5[i].c_str());
    }
 
 
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER | SDL_INIT_AUDIO) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    SDL_Texture* tex_screenshot;
    int my_image_width, my_image_height;

    // From 2.0.18: Enable native IME.
    #ifdef SDL_HINT_IME_SHOW_UI
      SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
    #endif

    // Create window with SDL_Renderer graphics context
    //SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_FULLSCREEN | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow("Set IP Address for game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1920, 1280, window_flags);
    if (window == nullptr)
    {
        printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        return -1;
    }
    // add error checking for img ang fft files etc
   

    SDL_Renderer* bg_renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (bg_renderer == nullptr)
    {
        SDL_Log("Error creating SDL_Renderer!");
        return 0;
    }

    if (file_exists("screenshot.png") !="")
    {
        LoadTextureFromFile("screenshot.png", &tex_screenshot, my_image_width, my_image_height, bg_renderer);
        game_sshot = true;
    } else if (file_exists("screenshot.jpg") !="")
    {
        LoadTextureFromFile("screenshot.jpg", &tex_screenshot, my_image_width, my_image_height, bg_renderer);
        std::string bob = file_exists("screenshot.jpg");
        game_sshot = true;
        std::cout << "YES: " << bob << std::endl;
    }
    else
    {
        printf("No Screenshot");

    }

//   // if (game_sshot == LoadTextureFromFile("screenshot.jpg", &tex_screenshot, my_image_width, my_image_height, bg_renderer))
  //  {
    //    printf("Error: No Screenshot");
  ///      return false;
  //  }

    //setup image

     //Initialize PNG loading
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags)&imgFlags))
    {
        printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
        return false;
    }
    my_texture = IMG_LoadTexture(bg_renderer, "Assets/Images/battlezone.png");

    //Setup Sound
    if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
    {
        printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
        return false;
    }
    gHigh = Mix_LoadWAV( "Assets/Sounds/shoot.wav" );
    if( gHigh == NULL )
    {
        printf( "Failed to load sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
        return false;
    }

    //SDL_RendererInfo info;
    //SDL_GetRendererInfo(renderer, &info);
    //SDL_Log("Current SDL_Renderer: %s", info.name);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    //Disabled so keys can be mapped with gptokeyb instead
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();


    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(window, bg_renderer);
    ImGui_ImplSDLRenderer2_Init(bg_renderer);

    //Using open source Roboto Medium. Remark the line below to use defaul 13 point terminal font.
    // Add error checking and default to system font.
    io.Fonts->AddFontFromFileTTF("Assets/Fonts/Roboto-Medium.ttf", 18.0f);
    
    // Colour state for fillrecct
    //ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    //Default Font Colour
    ImGuiStyle* style = &ImGui::GetStyle();
    style->Colors[ImGuiCol_Text] = ImVec4(0.29f, 0.96f, 0.15f, 1.0f);

    // Main loop
    while (!done)
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);

            switch (event.type) {
                case SDL_QUIT:
                    done = true;
                    break;
                case SDL_WINDOWEVENT: // && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window):
                    if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window)){
                        done =true;
                    }
                        break;
                case SDL_TEXTINPUT:
                    input += event.text.text;
                    std::cout  << "SDL Key: " << event.text.text << std::endl;
                    break;
			    case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_BACKSPACE && input.size()) {
                        input.pop_back();
                    }
                    break;
                /*case SDL_CONTROLLERBUTTONDOWN:
                    switch (event.cbutton.button) {
                        case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_START:
                        //std::cerr << "Start pressed!" << std::endl;
                            done = true;
                            show_ip_window = false;
                            return 0;
                            break;
                        case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_UP:
                        case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_DOWN:
                            Mix_PlayChannel( -1, gHigh, 0 );
                            break;
                    }*/
                break;
            }

        }
       
        // Start the Dear ImGui frame
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
 

        // Demonstrate create a window with multiple child windows.
        {
            bool p_open = true;
            
            //Full Screen IMGUI window
            ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
            ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
            ImGui::SetNextWindowBgAlpha(0.15f);
            ImGui::GetStyle().WindowMenuButtonPosition = ImGuiDir_None;
            //ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiCond_FirstUseEver);

            // Assign Window Title
            char buf[120];
            sprintf(buf, "Portmaster - Game Launcher - %s", arguments.title);
            
            if (ImGui::Begin(buf, &p_open)){
                // Left  Container    
                {              
                ImGui::BeginChild("left pane", ImVec2(150, 0), ImGuiChildFlags_Border);
                std::string texts[1] = {"Game Instructions"}; //,"Screenshot" ,"License", "Debug Info"
                //ImGui::SetWindowFocus();   
                if (ImGui::IsWindowAppearing())
                    ImGui::SetKeyboardFocusHere();
                 if (ImGui::Button("Start Game")|| (ImGui::IsKeyPressed(ImGuiKey_GamepadStart)))
                    {
                        done = true;
                        show_ip_window = false;
                        return 0;
                    }
                if (ImGui::Button("Game Options")|| (ImGui::IsKeyPressed(ImGuiKey_F3)))
                    {
                        show_ip_window = true;
                        //return 0;
                    }                   

                //  for (int i : texts)
                for (int i = 0; i < 1; i++)
                    {
                        // FIXME: Good candidate to use ImGuiSelectableFlags_SelectOnNav
                        char label[128];
                        sprintf(label, texts[i].c_str());
                       // if (ImGui::Selectable(label, selected == i))
                          //  selected = i;
                    }

           
                ImGui::EndChild();
                }
                ImGui::SameLine();

                // Right ContainerS
                {
                    ImGui::BeginGroup();
                    ImGui::BeginChild("item view", ImVec2(0,0));
                    ImGui::Separator();
                    Mix_Volume(-1, 1);
                    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_UpArrow)) || ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_DownArrow)) 
                    || ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_GamepadDpadDown)) || ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_GamepadDpadUp)))
                    {
                        Mix_PlayChannel( -1, gHigh, 0 );
                    }
                    if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_AutoSelectNewTabs))
                    {

                        if (ImGui::BeginTabItem("Description"))
                        {
                           
                            if (selected == 0)
                            {
                                if (game_sshot){
                                    // Load screenshot image.
                                    ImGui::Image((void*) tex_screenshot, ImVec2(my_image_width, my_image_height));
                                }
                                Markdown(readme);
                            }
                            
                            /*if (selected == 1)
                            {
                                // Load screenshot image.
                                ImGui::Text("ID: 0123456789");
                                ImGui::Text("pointer = %p", tex_screenshot);
                                ImGui::Text("size = %d x %d", my_image_width, my_image_height);
                                ImGui::Image((void*) tex_screenshot, ImVec2(my_image_width, my_image_height));
                            }

                             if (selected ==2)
                            {
                                ImGui::TextWrapped("Output: %s", licence.c_str());
                            }
                            if (selected == 3)
                            {
                                ImGui::Checkbox("Show Full Debug Info", &full_debug);
                                ImGui::Separator();
                                ImGui::TextWrapped("log file output: %s", log_debug.c_str());
                                if (full_debug)
                                {
                                    ImGui::Separator();
                                    ImGui::TextWrapped("file output: %s", file_debug.c_str());
                                    ImGui::Separator();
                                    ImGui::TextWrapped("ldd output: %s", ldd_debug.c_str());
                                    ImGui::Separator();
                                }
                            }*/
                            ImGui::EndTabItem();
                            ImGui::Separator();                        
                        }
                     
                    ImGui::EndTabBar();   
                    }

                    ImGui::EndChild();      
                    ImGui::EndGroup();                    
                }
            }
            ImGui::End();
        }

        if (show_ip_window)
        {

            ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
            ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
            ImGui::Begin("Change IP Address", &show_ip_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)

            static char buf1[128] = "Edit Text Test";
            
            //static ImGui::VirtualKeyboardFlags virtualKeyboardFlags = ImGui::VirtualKeyboardFlags_ShowBaseBlock;
            //ShowKeypadBlock //ShowBaseBlock // ShowAllBlocks // ShowAllBlocks displays all the keyboard parts
            //SDL_StartTextInput() ;

            ImGui::InputText("##Input", buf1, IM_ARRAYSIZE(buf1));
            

            //ImGui::VirtualKeyboard(virtualKeyboardFlags, ImGui::KLL_QWERTY,ImGui::KPL_ISO);        
       
            
            // Creates space
            Markdown(instructions);
            // Creates space
            ImGui::Dummy(ImVec2(0.0f, 20.0f));
            ImGui::Text("Number of Players:-");
            ImGui::SameLine();
            //ImGui::SliderScalar("", ImGuiDataType_U8, &iplayer, &u8_min, &u8_max, "%u");
            ImGui::DragInt("##items_count", &iplayer, 0.01f, 1, mplayer);
            ImGui::Dummy(ImVec2(0.0f, 20.0f));

            //ImGui::SetCursorPos(ImVec2(0,350));   // Place Button
            ImGui::Text("Currently set Server IP Address: %s",ips_load.c_str());
            ImGui::Text("Set Server IP Adress:-");
            ImGui::DragInt4("Server", vec4i_svr1, 1, 1, 255);
            if (iplayer==1)
            {
                ImGui::Separator();
                ImGui::Text("Currently set Player %u IP Address: %s",iplayer, ip1_load.c_str());
                ImGui::DragInt4("Player 1 IP", vec4i_ply1, 1, 1, 255);
                
                if (ImGui::Button("Save IP Address & Start Game",ImVec2(347,50))|| (ImGui::IsKeyPressed(ImGuiKey_F2)))
                {
                    ips_load = write_file("ip_server.txt",iplayer, vec4i_svr1);
                    ip1_load = write_file("ip1.txt",iplayer, vec4i_ply1);
                    done = true;
                    show_ip_window =false;
                    return 121;
                }
            }
            if (iplayer==2)
            {
                ImGui::Separator();
                ImGui::Text("Currently set Player 1 IP Address: %s", ip1_load.c_str());
                ImGui::DragInt4("Player 1 IP", vec4i_ply1, 1, 1, 255);
                ImGui::Separator();
                ImGui::Text("Currently set Player %u IP Address: %s",iplayer, ip2_load.c_str());
                ImGui::DragInt4("Player 2 IP", vec4i_ply2, 1, 1, 255);
                if (ImGui::Button("Save IP Address & Start Game",ImVec2(347,50))|| (ImGui::IsKeyPressed(ImGuiKey_F2)))
                {
                //printf("%d.%d.%d.%d", vec4i_ply1[0],vec4i_ply1[1],vec4i_ply1[2],vec4i_ply1[3]);
                ips_load = write_file("ip_server.txt",iplayer, vec4i_svr1);
                ip1_load = write_file("ip1.txt",iplayer, vec4i_ply1);
                ip2_load = write_file("ip2.txt",iplayer, vec4i_ply2); 
                done = true;
                show_ip_window =false;
                return 122;
                }
            }
            if (iplayer==3)
            {
                ImGui::Separator();
                ImGui::Text("Currently set Player 1 IP Address: %s", ip1_load.c_str());
                ImGui::DragInt4("Player 1 IP", vec4i_ply1, 1, 1, 255);
                ImGui::Text("Currently set Player 2 IP Address: %s", ip2_load.c_str());
                ImGui::Separator();
                ImGui::DragInt4("Player 2 IP", vec4i_ply2, 1, 1, 255);
                ImGui::Separator();
                ImGui::Text("Currently set Player %u IP Address: %s",iplayer, ip3_load.c_str());
                ImGui::DragInt4("Player 3 IP", vec4i_ply3, 1, 1, 255);
                if (ImGui::Button("Save IP Address & Start Game",ImVec2(347,50))|| (ImGui::IsKeyPressed(ImGuiKey_F2)))
                {
                //printf("%d.%d.%d.%d", vec4i_ply1[0],vec4i_ply1[1],vec4i_ply1[2],vec4i_ply1[3]);
                ips_load = write_file("ip_server.txt",iplayer, vec4i_svr1);
                ip1_load = write_file("ip1.txt",iplayer, vec4i_ply1);
                ip2_load = write_file("ip2.txt",iplayer, vec4i_ply2);
                ip3_load = write_file("ip3.txt",iplayer, vec4i_ply3); 
                done = true;
                show_ip_window =false;
                return 123;    
                }
            }
            if (iplayer==4)
            {
                ImGui::Separator();
                ImGui::Text("Currently set Player 1 IP Address: %s", ip1_load.c_str());
                ImGui::DragInt4("Player1 IP", vec4i_ply1, 1, 1, 255);
                ImGui::Separator();
                ImGui::Text("Currently set Player 2 IP Address: %s", ip2_load.c_str());
                ImGui::DragInt4("Player2 IP", vec4i_ply2, 1, 1, 255);
                ImGui::Separator();
                ImGui::Text("Currently set Player 3 IP Address: %s", ip3_load.c_str());
                ImGui::DragInt4("Player3 IP", vec4i_ply3, 1, 1, 255);
                ImGui::Separator();
                ImGui::Text("Currently set Player %u IP Address: %s",iplayer, ip4_load.c_str());
                ImGui::DragInt4("Player4 IP", vec4i_ply4, 1, 1, 255);
                if (ImGui::Button("Save IP Address & Start Game",ImVec2(347,50))|| (ImGui::IsKeyPressed(ImGuiKey_F2)))
                {
                //printf("%d.%d.%d.%d", vec4i_ply1[0],vec4i_ply1[1],vec4i_ply1[2],vec4i_ply1[3]);
                ips_load = write_file("ip_server.txt",iplayer, vec4i_svr1);
                ip1_load = write_file("ip1.txt",iplayer, vec4i_ply1);
                ip2_load = write_file("ip2.txt",iplayer, vec4i_ply2);
                ip3_load = write_file("ip3.txt",iplayer, vec4i_ply3); 
                ip4_load = write_file("ip4.txt",iplayer, vec4i_ply4);
                done = true;
                show_ip_window =false;
                return 124;
                }
            }
            if (ImGui::Button("Close and no Save",ImVec2(347,50))|| (ImGui::IsKeyPressed(ImGuiKey_F2)))
            {
                show_ip_window =false;
            }


            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        SDL_RenderSetScale(bg_renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
        //Background Fill Colour
        //SDL_SetRenderDrawColor(bg_renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
        SDL_RenderClear(bg_renderer);
        SDL_RenderCopy(bg_renderer, my_texture, NULL, NULL);
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
        SDL_RenderPresent(bg_renderer);       
    }

    // Cleanup
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    Mix_FreeChunk( gHigh );
    gHigh = NULL;
    SDL_DestroyTexture(my_texture);
    Mix_Quit();
    IMG_Quit();
    SDL_DestroyRenderer(bg_renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
