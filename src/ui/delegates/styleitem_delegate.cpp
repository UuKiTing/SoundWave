#include "model_roles.h"
#include "styleitem_delegate.h"
#include "image_loader_global.h"
#include "coverutils.h"
#include "song_manager.h"
#include "image_utils.h"
#include <QPainter>
#include <QApplication>
#include <QMouseEvent>
#include <QSortFilterProxyModel>
#include <QPainterPath>
#include <QFile>
#include <QVariant>

StyleItemDelegate::StyleItemDelegate(QObject *parent)
    : QStyledItemDelegate{parent}
{
    connect(&ImageLoaderGlobal::getInstance(), &ImageLoaderGlobal::imageLoaded, this, [this](int song_id, const QString& path, QVariant var){
        QSortFilterProxyModel* proxy = var.value<QSortFilterProxyModel*>();
        if(proxy){
            for (int row = 0; row < proxy->rowCount(); ++row) {
                QModelIndex proxyIdx = proxy->index(row, 0);
                if (proxyIdx.data(Roles::CoverPath).toString() == path) {
                    emit proxy->dataChanged(proxyIdx, proxyIdx, {Roles::CoverPath});
                }
            }
        }
    });
}

QRect StyleItemDelegate::iconRectFor(const QRect &r, int iconSize)
{
    int margin = (r.height() - iconSize) / 2;
    return QRect(r.left() + margin,
                 r.top() + margin,
                 iconSize,
                 iconSize);
}


QRect StyleItemDelegate::durationRectFor(const QRect &r, int durWidth, int durMarginRight)
{
    return QRect(r.right() - durMarginRight - durWidth,
                 r.top(),
                 durWidth,
                 r.height());
}


QRect StyleItemDelegate::favBtnRectFor(const QRect &r, int btnSize, int btnMarginRight)
{
    return QRect(r.right() - btnMarginRight - btnSize,
                 r.center().y() - btnSize / 2,
                 btnSize,
                 btnSize);
}


QRect StyleItemDelegate::markIconRectFor(const QRect &r, int btnSize, int btnMarginRight)
{
    return QRect(r.right() - btnMarginRight - btnSize,
                 r.center().y() - btnSize / 2,
                 btnSize,
                 btnSize);
}

void StyleItemDelegate::textRectsFor(QRect &titleRect, QRect &artistRect, const QRect &r,int marginLeft, int width)
{
    int margin = r.height() / 10;
    int height = (r.height() - 2 * margin) / 2;
    titleRect = QRect(marginLeft,
                      r.top() + margin,
                      width,
                      height);

    artistRect = QRect(marginLeft,
                       titleRect.bottom(),
                       width,
                       height);
}

void StyleItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(!option.widget || !option.widget->isVisible()){
        return;
    }

    const bool isPlaying = index.data(Roles::IsPlaying).toBool();
    const QString title = index.data(Roles::Title).toString();
    const QString artist = index.data(Roles::Artist).toString();
    const bool isFavorite = index.data(Roles::IsFavorite).toBool();

    const QColor blackColor = Qt::black;
    const QColor selectdColor(0x32, 0x59, 0xCE);
    const QColor greyColor(0x99, 0x99, 0x99);

    painter->save();
    if (option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, QColor(0xdfdfdf));
    } else if (option.state & QStyle::State_MouseOver) {
        painter->fillRect(option.rect, QColor(0xf1f1f1));
    } else {
        painter->fillRect(option.rect, Qt::white);
    }
    painter->restore();

    // 图标
    QString path = index.data(Roles::CoverPath).toString();
    QRect iconRect = iconRectFor(option.rect, ICON_SIZE);

    QSortFilterProxyModel* proxy = qobject_cast<QSortFilterProxyModel*>(const_cast<QAbstractItemModel*>(index.model()));

    int song_id = index.data(Roles::Id).toInt();
    CoverUtils::loadCoverAsync(song_id, path, QSize(ICON_SIZE, ICON_SIZE), 5,
                               [painter, iconRect](const QPixmap& pix){
                                    painter->drawPixmap(iconRect, pix);
                                },
                               [painter, iconRect, &option](){
                                    painter->drawPixmap(iconRect, defaultCover());
                                },
                               QVariant::fromValue(proxy)
    );


    // 右侧保留区域：时长 + 按钮
    QRect durRect = durationRectFor(option.rect, DUR_WIDTH, DUR_MARGIN_RIGHT);
    QRect favBtnRect = favBtnRectFor(option.rect, BUTTON_SIZE, BUTTON_MARGIN_RIGHT);
    QRect markIconRect = markIconRectFor(option.rect, BUTTON_SIZE, MARK_ICON_MARGIN_RIGHT);


    // 文本区域
    int textLeft = iconRect.right() + TEXT_MARGIN_LEFT;
    QRect titleRect, artistRect;
    textRectsFor(titleRect, artistRect, option.rect, textLeft, (favBtnRect.left() - textLeft) / 2);


    const QColor &titleColor = isPlaying ? selectdColor : blackColor;
    const QColor &subColor = isPlaying ? selectdColor : greyColor;

    // 绘制歌名
    painter->save();
    QFont titleFont = painter->font();
    titleFont.setPixelSize(16);
    titleFont.setBold(true);
    painter->setFont(titleFont);
    painter->setPen(titleColor);

    QFontMetrics titleMetrics(titleFont);
    QString elidedTitle = titleMetrics.elidedText(title, Qt::ElideRight, titleRect.width());
    painter->drawText(titleRect,
                      Qt::AlignLeft | Qt::AlignVCenter,
                      elidedTitle);
    painter->restore();


    // 绘制歌手 + 时长
    painter->save();
    QFont subFont = painter->font();
    subFont.setPixelSize(14);
    painter->setFont(subFont);
    painter->setPen(subColor);

    QFontMetrics subMetrics(subFont);
    QString elidedArtist = subMetrics.elidedText(artist, Qt::ElideRight, artistRect.width());
    painter->drawText(artistRect,
                      Qt::AlignLeft | Qt::AlignVCenter,
                      elidedArtist);
    painter->drawText(durRect,
                      Qt::AlignRight | Qt::AlignVCenter,
                      toDurationString(index.data(Roles::Duration).toInt()));
    painter->restore();

    // 绘制收藏按钮
    painter->save();
    QIcon favIcon = isFavorite ? QIcon(":/icon/love.png") : QIcon(":/icon/dislove.png");
    favIcon.paint(painter, favBtnRect);
    painter->restore();


    painter->save();
    SongSource source = index.data(Roles::Source).value<SongSource>();
    QIcon markIcon = (source == SongSource::Local) ? QIcon("://icon/local.png") : QIcon(":/icon/remote.png");
    markIcon.paint(painter, markIconRect);
    painter->restore();


    int progress = index.data(Roles::ProgressValue).toInt();

    if(progress == -1){
        return;
    }

    QRect itemRect = option.rect;

    int barHeight = 3;
    int margin = 4;

    QRect progressRect(
        itemRect.left() + margin,
        itemRect.bottom() - barHeight - margin,
        itemRect.width() - 2 * margin,
        barHeight
        );

    QStyleOptionProgressBar barOption;
    barOption.initFrom(option.widget);
    barOption.rect = progressRect;
    barOption.maximum = 3;
    barOption.progress = progress;
    barOption.textVisible = true;
    barOption.textAlignment = Qt::AlignCenter;
    barOption.state |= QStyle::State_Horizontal;

    QApplication::style()->drawControl(
        QStyle::CE_ProgressBar, &barOption, painter, option.widget);
}


QSize StyleItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSize(-1, 70);
}


bool StyleItemDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if (event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        QRect btnRect = favBtnRectFor(option.rect, BUTTON_SIZE, BUTTON_MARGIN_RIGHT);

        if (btnRect.contains(mouseEvent->pos())) {
            bool isFavo = index.data(Roles::IsFavorite).toBool();

            emit songCollected(!isFavo, index);

            return true;
        }
    }
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

