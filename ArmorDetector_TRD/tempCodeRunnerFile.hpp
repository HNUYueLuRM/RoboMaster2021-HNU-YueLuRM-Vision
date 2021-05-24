{
        unsigned char lut[256];
        for(int i=0;i < 256;i++)
        {
            lut[i] = saturate_cast<uchar>(pow((float)i/255.0,param.gamma_redress_value) * 255.0f);
        }
        gamma_img = raw_img.clone();
        int channels = raw_img.channels();
        switch(channels)
        {
            case 1:
            {
                MatIterator_<uchar> it = gamma_img.begin<uchar>();
                MatIterator_<uchar> end = gamma_img.end<uchar>();
                while(it != end)
                {
                    *it = lut[(*it)];
                    it ++;
                }
                break;
            }
            case 3:
                {
                    MatIterator_<Vec3b> it = gamma_img.begin<Vec3b>();
                    MatIterator_<Vec3b> end = gamma_img.end<Vec3b>();
                    while(it != end)
                    {
                        (*it)[0] = lut[(*it)[0]];
                        (*it)[1] = lut[(*it)[1]];
                        (*it)[2] = lut[(*it)[2]];
                        it ++;
                    }
                break;
                }
            default:
                break;
        }
    }