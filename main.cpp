#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <wx/wx.h>
#include "custom_image_class.hpp"

std::vector<std::string> returnImagePathsFromGivenDirectory(std::string& directory_path) {
   std::vector<std::string> file_strings;
   for (const auto& entry : std::filesystem::directory_iterator(directory_path)) {
      if (!entry.is_regular_file()) {
         continue;
      }
      if (entry.path().extension() == ".png") {
         file_strings.push_back(entry.path());
      }
   }
   return file_strings;
}

std::vector<CustomImage> getImages(std::vector<std::string>& image_paths) {
   std::vector<CustomImage> images;
   for (std::string& image_path : image_paths) {
      images.emplace_back(image_path);
   }
   return images;
}



class MyApp : public wxApp {
public:
   virtual bool OnInit();
};

wxIMPLEMENT_APP(MyApp);

#include "commands.hpp"

class MainFrame : public wxFrame {
public:
   int current_image_index;
   wxStaticBitmap* currently_displayed_image;
   wxPanel* image_panel;
   wxButton* previous_button;
   wxButton* next_button;
   wxButton* rotate_left_button;
   wxButton* rotate_right_button;
   wxStaticText* index_display;
   std::vector<CustomImage> images;

   MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

   void setImageAndUpdate() {
      currently_displayed_image->SetBitmap(*images[current_image_index].bitmap);
      image_panel->InvalidateBestSize();
      image_panel->GetParent()->Layout();
   }

   void setImageAndUpdate(wxBitmap& bitmap) {
      currently_displayed_image->SetBitmap(bitmap);
      image_panel->InvalidateBestSize();
      image_panel->GetParent()->Layout();
   }

   void updateDisplayIndexText() {
      index_display->SetLabel(std::to_string(current_image_index + 1) + "|" + std::to_string(images.size()));
      index_display->GetParent()->Layout();
   }

   void initButtons() {
      previous_button->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) mutable {
                                                                           jumpToPreviousImages(this->images, this->current_image_index, 1);
                                                                           setImageAndUpdate();
                                                                           updateDisplayIndexText();
                                                                          });
      next_button->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) mutable {
                                                                       jumpToNextImages(this->images, this->current_image_index, 1);
                                                                       setImageAndUpdate();
                                                                       updateDisplayIndexText();
                                                                      });
      rotate_left_button->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) mutable {
         wxImage img = images[current_image_index].bitmap->ConvertToImage();
         images[current_image_index].angle -= 90.f;
         wxImage rotate_img = img.Rotate(images[current_image_index].getRadAngle(), getCenter(img), true);
         wxBitmap rotated_bmp(rotate_img);
         //currently_displayed_image->SetBitmap(rotated_bmp);
         //currently_displayed_image->InvalidateBestSize();
         //currently_displayed_image->GetParent()->Layout();
         setImageAndUpdate(rotated_bmp);
         std::cout << "good\n";
      });
      rotate_right_button->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) mutable {
         wxImage img = images[current_image_index].bitmap->ConvertToImage();
         images[current_image_index].angle += 90.f;
         wxImage rotate_img = img.Rotate(images[current_image_index].getRadAngle(), getCenter(img), true);
         wxBitmap rotated_bmp(rotate_img);
         setImageAndUpdate(rotated_bmp);
      });
   }
};

bool MyApp::OnInit() {
   wxInitAllImageHandlers();
   MainFrame* frame = new MainFrame("Hello World", wxDefaultPosition, wxSize(1280,720));
   frame->Show(true);
   return true;
}

MainFrame::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size) : wxFrame(nullptr, wxID_ANY, title, pos, size) {
   this->current_image_index = 0;

   wxPanel* info_panel = new wxPanel(this, wxID_ANY);
   info_panel->SetBackgroundColour(wxColor(255,0,255));
   this->image_panel = new wxPanel(this, wxID_ANY);

   wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
   wxBoxSizer* info_panel_sizer = new wxBoxSizer(wxHORIZONTAL);
   wxFlexGridSizer* button_grid = new wxFlexGridSizer(2,5,2,2);

   // file path goes here:
   std::string directory_path = "";
   std::vector<std::string> image_paths = returnImagePathsFromGivenDirectory(directory_path);

   this->images = getImages(image_paths);
   this->currently_displayed_image = new wxStaticBitmap(image_panel, wxID_ANY, *images[current_image_index].bitmap);

   this->previous_button = new wxButton(info_panel, wxID_ANY, "Previous");
   this->next_button = new wxButton(info_panel, wxID_ANY, "Next");
   this->rotate_left_button = new wxButton(info_panel, wxID_ANY, "Rotate Left");
   this->rotate_right_button = new wxButton(info_panel, wxID_ANY, "Rotate Right");

   this->index_display = new wxStaticText(info_panel, wxID_ANY, "Hello World");

   initButtons();

   button_grid->Add(previous_button);
   button_grid->Add(next_button);
   button_grid->Add(rotate_left_button);
   button_grid->Add(rotate_right_button);
   info_panel_sizer->Add(button_grid, 0, wxALL, 10);

   info_panel_sizer->AddStretchSpacer(1);
   info_panel_sizer->Add(index_display, 1, wxALIGN_CENTER);
   info_panel_sizer->AddStretchSpacer(1);

   info_panel->SetSizer(info_panel_sizer);

   main_sizer->Add(info_panel, 0, wxEXPAND | wxLEFT, 5);
   main_sizer->Add(image_panel, 0, wxALL | wxCENTER, 5);

   this->SetSizer(main_sizer);
}
