
wxPoint getCenter(wxImage& img) {
   return wxPoint(img.GetWidth() / 2, img.GetHeight() / 2);
}

class CustomImage {
public:
   wxBitmap bitmap;
   wxBitmap display_bitmap;
   float zoom_amount;
   float angle;
   bool flip_vertically, flip_horizontally;

   CustomImage(std::string& image_path) : bitmap(image_path, wxBITMAP_TYPE_ANY), display_bitmap(image_path, wxBITMAP_TYPE_ANY) {
      this->zoom_amount = 1.f;
      this->angle = 0.f;
      this->flip_vertically = false;
      this->flip_horizontally = false;
   }

   wxImage getNewImage(std::vector<CustomImage>& images, int& current_image_index, wxImage& img) {
      handleFlips(img);
      handleRotation(img);
      handleScale(images, current_image_index, img);
      return img;
   }

   double getRadAngle(double angle) {
      return wxDegToRad(angle);
   }

private:
   void handleScale(std::vector<CustomImage>& images, int& current_image_index, wxImage& img) {
      img = img.Scale(img.GetWidth() * images[current_image_index].zoom_amount, img.GetHeight() * images[current_image_index].zoom_amount, wxIMAGE_QUALITY_HIGH);
   }

   void handleRotation(wxImage& img) {
      img = img.Rotate(getRadAngle(angle), getCenter(img), true);
   }

   void handleFlips(wxImage& img) {
      if (flip_vertically) {
         img = img.Mirror(false);
      }
      if (flip_horizontally) {
         img = img.Mirror(true);
      }
   }
};
