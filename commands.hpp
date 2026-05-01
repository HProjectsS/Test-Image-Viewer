

void jumpToPreviousImages(std::vector<CustomImage>& images, int& current_image_index, int number_of_times) {
   for (int i = 0; i < number_of_times; i++) {
      current_image_index--;
      if (current_image_index < 0) {
         current_image_index = images.size() - 1;
      }
   }
}

void jumpToNextImages(std::vector<CustomImage>& images, int& current_image_index, int number_of_times) {
   for (int i = 0; i < number_of_times; i++) {
      current_image_index = (current_image_index + 1) % images.size();
   }
}
