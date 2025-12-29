/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

/*   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         */

#ifndef DIALED_H
#define DIALED_H

#include "component.h"
#include "e-element.h"

class Dialed : public Component
{
public:
    Dialed(std::string type, std::string id);
    ~Dialed();
    std::vector<comProperty*> dialProps();
    bool slider() { return m_slider; }
    void setSlider(bool s);
    double scale() { return m_scale; }
    void setScale(double s);
    //virtual void setHidden(bool hid, bool hidArea = false, bool hidLabel = false) override;
    virtual void setLinkedValue(double v, int i = 0) override;
    virtual void dialChanged();
protected:
    virtual void updateProxy() { ; }
    double m_scale;
    bool m_needUpdate ;
    bool m_slider;
};
#endif
