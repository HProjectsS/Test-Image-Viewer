#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <wx/wx.h>
#include "custom_image_class.hpp"

enum Ids {
   info_panel = wxID_LAST - 1,
   image_panel,
   currently_displayed_image,
   previous_button,
   next_button,
   rotate_left_button,
   rotate_right_button,
   flip_horizontally_button,
   flip_vertically_button,
   zoom_in_button,
   zoom_out_button,
   show_info_panel,
   index_display,
};

std::vector<std::string> returnImagePathsFromGivenDirectory(std::string& directory_path) {
   std::vector<std::string> file_strings;
   for (const auto& entry : std::filesystem::directory_iterator(directory_path)) {
      if (!entry.is_regular_file()) {
         continue;
      }
      if (entry.path().extension() == ".png" || entry.path().extension() == ".jpg") {
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
   float zoom_amount;
   int current_image_index;
   unsigned int info_panel_in_main_sizer_index;
   bool flip_image_horizontally;
   bool flip_image_vertically;

   wxMenuBar* menu_bar;

   wxStaticBitmap* currently_displayed_image;
   wxPanel* info_panel;
   wxPanel* image_panel;

   wxBoxSizer* main_sizer;
   wxFlexGridSizer* button_grid;

   wxButton* previous_button;
   wxButton* next_button;
   wxButton* rotate_left_button;
   wxButton* rotate_right_button;
   wxButton* flip_horizontally_button;
   wxButton* flip_vertically_button;
   wxButton* zoom_in_button;
   wxButton* zoom_out_button;
   wxStaticText* index_display;

   std::vector<CustomImage> images;

   MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

   void setImageAndUpdate() {
      currently_displayed_image->SetBitmap(images[current_image_index].display_bitmap);
      image_panel->InvalidateBestSize();
      image_panel->GetParent()->Layout();
   }

private:
   void onView_ShowInfoPanel(wxCommandEvent& ev) {
      if (this->info_panel->IsShown()) {
         this->info_panel->Hide();
         this->main_sizer->Detach(info_panel);
         this->main_sizer->Layout();
      } else {
         main_sizer->Insert(info_panel_in_main_sizer_index, info_panel, 0, wxEXPAND, 5);
         this->info_panel->Show();
         this->main_sizer->Layout();
      }
   }

   void fitImageToScreen() {}

   void onFile_Open(wxCommandEvent& ev) {
      wxFileDialog openFileDialog(
         this,
         "Open File",
         "", "",
         "PNG files (*.png)|*.png|JPEG files (*.jpg)|*.jpg",
         wxFD_OPEN | wxFD_FILE_MUST_EXIST
      );

      if (openFileDialog.ShowModal() == wxID_CANCEL) { return; }

      std::string file_path_str = openFileDialog.GetPath().ToStdString();
      std::filesystem::path file_path = file_path_str;
      std::filesystem::path dir_path = file_path.parent_path();

      initImages(dir_path.string());
      setImageAndUpdate();
   }

   void onFile_Exit(wxCommandEvent& ev) {
      Close(true);
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

   void initButtonCommands() {
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
         images[current_image_index].angle -= 90.f;
         wxImage img = images[current_image_index].bitmap.ConvertToImage();
         wxImage new_img = images[current_image_index].getNewImage(this->images, this->current_image_index, img);
         images[current_image_index].display_bitmap = wxBitmap(new_img);
         setImageAndUpdate();
      });
      rotate_right_button->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) mutable {
         images[current_image_index].angle += 90.f;
         wxImage img = images[current_image_index].bitmap.ConvertToImage();
         wxImage new_img = images[current_image_index].getNewImage(this->images, this->current_image_index, img);
         images[current_image_index].display_bitmap = wxBitmap(new_img);
         setImageAndUpdate();
      });
      flip_horizontally_button->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) mutable {
         images[current_image_index].flip_horizontally = !images[current_image_index].flip_horizontally;
         wxImage img = images[current_image_index].bitmap.ConvertToImage();
         wxImage new_img = images[current_image_index].getNewImage(this->images, this->current_image_index, img);
         images[current_image_index].display_bitmap = wxBitmap(new_img);
         setImageAndUpdate();
      });
      flip_vertically_button->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) mutable {
         images[current_image_index].flip_vertically = !images[current_image_index].flip_vertically;
         wxImage img = images[current_image_index].bitmap.ConvertToImage();
         wxImage new_img = images[current_image_index].getNewImage(this->images, this->current_image_index, img);
         images[current_image_index].display_bitmap = wxBitmap(new_img);
         setImageAndUpdate();
      });
      zoom_in_button->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) mutable {
         images[current_image_index].zoom_amount *= this->zoom_amount;
         wxImage img = images[current_image_index].bitmap.ConvertToImage();
         wxImage new_img = images[current_image_index].getNewImage(this->images, this->current_image_index, img);
         images[current_image_index].display_bitmap = wxBitmap(new_img);
         setImageAndUpdate();
      });
      zoom_out_button->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) mutable {
         images[current_image_index].zoom_amount /= this->zoom_amount;
         wxImage img = images[current_image_index].bitmap.ConvertToImage();
         wxImage new_img = images[current_image_index].getNewImage(this->images, this->current_image_index, img);
         images[current_image_index].display_bitmap = wxBitmap(new_img);
         setImageAndUpdate();
      });
   }

   void initImages() {
      // add path here:
      std::string directory_path = "";
      std::vector<std::string> image_paths = returnImagePathsFromGivenDirectory(directory_path);

      this->images = getImages(image_paths);

      if (currently_displayed_image == nullptr) {
         this->currently_displayed_image = new wxStaticBitmap(image_panel, Ids::currently_displayed_image, images[current_image_index].bitmap);
      } else  {
         this->currently_displayed_image->SetBitmap(images[current_image_index].display_bitmap);
      }
   }

   void initImages(std::string director_path_str) {
      std::string directory_path = director_path_str;
      std::vector<std::string> image_paths = returnImagePathsFromGivenDirectory(directory_path);

      this->current_image_index = 0;

      this->images = getImages(image_paths);
      if (currently_displayed_image == nullptr) {
         this->currently_displayed_image = new wxStaticBitmap(image_panel, Ids::currently_displayed_image, images[current_image_index].bitmap);
      } else {
         this->currently_displayed_image->SetBitmap(images[current_image_index].display_bitmap);
      }
   }

   wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_MENU(Ids::show_info_panel, MainFrame::onView_ShowInfoPanel)
wxEND_EVENT_TABLE();

bool MyApp::OnInit() {
   wxInitAllImageHandlers();
   MainFrame* frame = new MainFrame("Hello World", wxDefaultPosition, wxSize(1280,720));
   frame->Show(true);
   return true;
}

MainFrame::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size) : wxFrame(nullptr, wxID_ANY, title, pos, size) {
   this->zoom_amount = 1.5f;
   this->flip_image_horizontally = true;
   this->flip_image_vertically = false;
   this->current_image_index = 0;
   this->info_panel_in_main_sizer_index = 0;

   this->currently_displayed_image = nullptr;

   this->info_panel = new wxPanel(this, Ids::info_panel);
   info_panel->SetBackgroundColour(wxColor(255,0,255));
   this->image_panel = new wxPanel(this, Ids::image_panel);

   this->menu_bar = new wxMenuBar();

   wxMenu* file_menu = new wxMenu();
   file_menu->Append(wxID_OPEN, "&Open\tCtrl-O");
   file_menu->AppendSeparator();
   file_menu->Append(wxID_EXIT, "&Exit\tAlt-X");

   wxMenu* view_menu = new wxMenu();
   view_menu->Append(Ids::show_info_panel, "&Show Info Bar");

   menu_bar->Append(file_menu, "&File");
   menu_bar->Append(view_menu, "&View");

   SetMenuBar(menu_bar);

   Bind(wxEVT_MENU, &MainFrame::onFile_Open, this, wxID_OPEN);
   Bind(wxEVT_MENU, &MainFrame::onFile_Exit, this, wxID_EXIT);

   Bind(wxEVT_MENU, &MainFrame::onView_ShowInfoPanel, this, Ids::show_info_panel);

   this->main_sizer = new wxBoxSizer(wxVERTICAL);
   wxBoxSizer* info_panel_sizer = new wxBoxSizer(wxHORIZONTAL);
   this->button_grid = new wxFlexGridSizer(2,4,2,2);

   this->initImages();

   this->previous_button = new wxButton(info_panel, Ids::previous_button, "Previous");
   this->next_button = new wxButton(info_panel, Ids::next_button, "Next");
   this->rotate_left_button = new wxButton(info_panel, Ids::rotate_left_button, "Rotate Left");
   this->rotate_right_button = new wxButton(info_panel, Ids::rotate_right_button, "Rotate Right");
   this->flip_horizontally_button = new wxButton(info_panel, Ids::flip_horizontally_button, "Flip Horizontally");
   this->flip_vertically_button = new wxButton(info_panel, Ids::flip_vertically_button, "Flip Vertically");
   this->zoom_in_button = new wxButton(info_panel, Ids::zoom_in_button, "Zoom In");
   this->zoom_out_button = new wxButton(info_panel, Ids::zoom_out_button, "Zoom Out");

   this->index_display = new wxStaticText(info_panel, Ids::index_display, "Hello World");

   initButtonCommands();

   button_grid->Add(previous_button, 1, wxEXPAND | wxALL, 0);
   button_grid->Add(next_button, 1, wxEXPAND | wxALL, 0);
   button_grid->Add(rotate_left_button, 1, wxEXPAND | wxALL, 0);
   button_grid->Add(rotate_right_button, 1, wxEXPAND | wxALL, 0);
   button_grid->Add(flip_horizontally_button, 1, wxEXPAND | wxALL, 0);
   button_grid->Add(flip_vertically_button, 1, wxEXPAND | wxALL, 0);
   button_grid->Add(zoom_in_button, 1, wxEXPAND | wxALL, 0);
   button_grid->Add(zoom_out_button, 1, wxEXPAND | wxALL, 0);
   info_panel_sizer->Add(button_grid, 1, wxALL | wxLEFT, 10);

   info_panel_sizer->AddStretchSpacer(1);
   info_panel_sizer->Add(index_display, 1, wxALIGN_CENTER);
   info_panel_sizer->AddStretchSpacer(1);

   info_panel->SetSizer(info_panel_sizer);

   main_sizer->Add(info_panel, 0, wxEXPAND, 5);
   main_sizer->Add(image_panel, 0, wxALL | wxCENTER, 5);

   this->SetSizer(main_sizer);
}
