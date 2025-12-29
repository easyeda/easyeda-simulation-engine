#ifndef CUSTOMBUTTON_H
#define CUSTOMBUTTON_H

class CustomButton {
public:
    CustomButton( int index) :m_checked(false) ,m_index(index){};
 
    bool isChecked(){
        return m_checked;
    }
    void setChecked( bool flag){
        m_checked = flag;
    }
    int getIndex(){
        return m_index;
    }

private:
    int m_index;
    bool m_checked;
};

#endif // CUSTOMBUTTON_H
