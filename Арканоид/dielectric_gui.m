function varargout = dielectric_gui(varargin)
% dielectric_gui Application M-file
%    dielectric_gui - launch dielectric GUI.
%    dielectric_gui('callback_name', ...) invoke the named callback.


if nargin == 0  % LAUNCH GUI
    global lightspeed;
    global omega1 omega2 nu1 nu2 lamda1 lamda2 resolution epsiloninfrared;
    global heditM1amu heditM2amu heditDensity heditEpsilonLight heditEpsilonIR;
    global heditResolution;
    global heditOmega1 heditOmega2;
    global heditNu1 heditNu2;
    global heditLamda1 heditLamda2;
    
    lightspeed=300000000;   % [m/s]
    omega1=10E11;  % [rad/s]
    omega2=10E14;  % [rad/s]
    nu1=omega2wave(omega1);    % [1/s]
    nu2=omega2wave(omega2);
    lamda1=omega2lamda(omega1); % [m]
    lamda2=omega2lamda(omega2);
    resolution = 10000;
    epsiloninfrared = 2;
    fontname = 'Arial CYR';
    
    fig = figure('Name','ƒиэлектрики','Numbertitle','off','Resize','on','Color',...
        get(0,'defaultUicontrolBackgroundColor'),'Menubar','none','Units','characters',...
        'Position',[2, 10, 70, 27]);
    %  нопки
    for i=1:4
        switch i
        case 1
            alkaline = 'Na';
        case 2
            alkaline = 'K';
        case 3
            alkaline = 'Rb';
        case 4
            alkaline = 'Cs';
        end
        try
            uicontrol('Style','text','String',alkaline,'Units','characters', ...
                'Position',[5.8+(i-1)*6.2 23.8 6.2 1.5],...
                'HorizontalAlignment','center','FontName',fontname);
        catch
            disp(lasterr);
        end
        for j=1:4
            switch j
            case 1
                halogen = 'F';
            case 2
                halogen = 'Cl';
            case 3
                halogen = 'Br';
            case 4
                halogen = 'I';
            end
            if i == 1
                try
                    uicontrol('Style','text','String',halogen,'Units','characters', ...
                        'Position',[1.8 21.5-(j-1)*2.4 4 2.4],...
                        'HorizontalAlignment','center','FontName',fontname);
                catch
                    disp(lasterr);
                end
            end
            try
                uicontrol('Style','pushbutton','String',[alkaline halogen],...
                    'Units','characters','Position',[5.8+(i-1)*6.2 21.5-(j-1)*2.4 6.2 2.4], ...
                    'Callback',['dielectric_gui(''pushElement'',gcbo)'],...
                    'FontName',fontname,'UserData',[alkaline ',' halogen]);
            catch
                errordlg(lasterr);
            end
        end
    end
    % –амка характеристик соединения
    try
        % —оздаем не обязательные элементы (надписи, рамки)
        uicontrol('Style','frame','Units','characters','Position',[1.8 0.3 40.2 12.5]);
        uicontrol('Style','text','String','’арактеристики соединения','Units','characters',...
            'Position',[3.8 12 30 1.4],'FontName',fontname);
        uicontrol('Style','text','String','M  :','Units','characters',...
            'Position',[3.8 9.92 6.2 1.6],'HorizontalAlignment','left','FontName',fontname);
        uicontrol('Style','text','String','1','Units','characters','Position',[5.3 10.2 1 1],...
            'HorizontalAlignment','left','FontName',fontname,'FontSize',7);
        uicontrol('Style','text','String','а.е.м.','Units','characters',...
            'Position',[29.8 9.92 6.2 1.6],'HorizontalAlignment','left','FontName',fontname);
        uicontrol('Style','text','String','M  :','Units','characters',...
            'Position',[3.8 7.6 6.2 1.6],'HorizontalAlignment','left','FontName',fontname);
        uicontrol('Style','text','String','2','Units','characters','Position',[5.3 7.9 1 1],...
            'HorizontalAlignment','left','FontName',fontname,'FontSize',7);
        uicontrol('Style','text','String','а.е.м.','Units','characters',...
            'Position',[29.8 7.6 6.2 1.6],'HorizontalAlignment','left','FontName',fontname);
        uicontrol('Style','text','String','r:','Units','characters',...
            'Position',[3.8 5.6 10.2 1.6],'HorizontalAlignment','left','FontName','Symbol','FontSize',10);
        uicontrol('Style','text','String','кг/м','Units','characters',...
            'Position',[29.8 5.3 10.2 1.6],'HorizontalAlignment','left','FontName',fontname);
        uicontrol('Style','text','String','3','Units','characters','Position',[34 5.5 2 1.6],...
            'HorizontalAlignment','left','FontName',fontname,'FontSize',6);
        uicontrol('Style','text','String','e','Units','characters','Position',[3.8 3.3 10.2 1.6],...
            'HorizontalAlignment','left','FontName','Symbol','FontSize',10);
        uicontrol('Style','text','String','беск.:','Units','characters',...
            'Position',[6 3 10.2 1.6],'HorizontalAlignment','left','FontName',fontname);
        uicontrol('Style','text','String','e','Units','characters','Position',[3.8 1 10.2 1.6],...
            'HorizontalAlignment','left','FontName','Symbol','FontSize',10);
        uicontrol('Style','text','String','и.к.:','Units','characters',...
            'Position',[6 .7 10.2 1.6],'HorizontalAlignment','left','FontName',fontname);
    catch
        disp(lasterr);
    end
    
    try
        % —оздаем обязательные элементы (едиты, кнопки)
        heditM1amu = uicontrol('Style','edit','Units','characters','Position',[11.8 9.92 16.2 1.6],...
            'HorizontalAlignment','right','BackgroundColor',[1 1 1],...
            'TooltipString','ћасса 1-го элемента [атомная единица массы]','FontName',fontname);
        heditM2amu = uicontrol('Style','edit','Units','characters','Position',[11.8 7.6 16.2 1.6],...
            'HorizontalAlignment','right','BackgroundColor',[1 1 1],...
            'TooltipString','ћасса 2-го элемента [атомная единица массы]','FontName',fontname);
        heditDensity = uicontrol('Style','edit','Units','characters','Position',[11.8 5.3 16.2 1.6],...
            'HorizontalAlignment','right','BackgroundColor',[1 1 1],...
            'TooltipString','ѕлотность кристалла [кг/куб.м]','FontName',fontname);
        heditEpsilonLight = uicontrol('Style','edit','Units','characters','Position',[15.8 3 12.2 1.6],...
            'HorizontalAlignment','right','BackgroundColor',[1 1 1],...
            'TooltipString','ƒиэлектрическая проницаемость (бесконечность)','FontName',fontname);    
        heditEpsilonIR = uicontrol('Style','edit','Units','characters','Position',[15.8 .7 12.2 1.6],...
            'HorizontalAlignment','right','BackgroundColor',[1 1 1],...
            'TooltipString','ƒиэлектрическая проницаемость (инфра-красный диапазон)','FontName',fontname);
        uicontrol('Style','pushbutton','String','OK','Units','characters','Position',[32.4 1.3 8 2.8], ...
            'Callback','dielectric_gui(''pushbuttonOK'')','FontName',fontname);
    catch
        errordlg(lasterr);
    end

    % –амка спектр
    try
        % —оздаем не обязательные элементы (надписи, рамки)
        uicontrol('Style','frame','Units','characters','Position',[31.8 13.7 34.2 10.2]);
        uicontrol('Style','text','String','„астотный диапазон','Units','characters',...
            'Position',[33.8 23.6 30 1],'FontName',fontname);
        
        uicontrol('Style','text','String','w','Units','characters','Position',[32.2 20.77 1.8 1.6],...
            'HorizontalAlignment','left','FontName','Symbol','FontSize',10);
        uicontrol('Style','text','String','(','Units','characters',...
            'Position',[33.8 20.77  2.2 1.6],'FontSize',10,'FontName',fontname); 
        uicontrol('Style','text','String','..','Units','characters',...
            'Position',[47.8 20.77  2.2 1.6],'FontSize',10,'FontName',fontname);
        uicontrol('Style','text','String',')','Units','characters',...
            'Position',[61.8 20.77  2.2 1.6],'FontSize',10,'FontName',fontname);

        uicontrol('Style','text','String','n','Units','characters','Position',[32.2 18.77 1.8 1.6],...
            'HorizontalAlignment','left','FontName','Symbol','FontSize',10);
        uicontrol('Style','text','String','(','Units','characters',...
            'Position',[33.8 18.77  2.2 1.6],'FontSize',10,'FontName',fontname); 
        uicontrol('Style','text','String','..','Units','characters',...
            'Position',[47.8 18.77  2.2 1.6],'FontSize',10,'FontName',fontname);
        uicontrol('Style','text','String',')','Units','characters',...
            'Position',[61.8 18.77  2.2 1.6],'FontSize',10,'FontName',fontname);

        uicontrol('Style','text','String','l','Units','characters','Position',[32.2 16.77 1.8 1.6],...
            'HorizontalAlignment','left','FontName','Symbol','FontSize',10);
        uicontrol('Style','text','String','(','Units','characters',...
            'Position',[33.8 16.77  2.2 1.6],'FontSize',10,'FontName',fontname); 
        uicontrol('Style','text','String','..','Units','characters',...
            'Position',[47.8 16.77  2.2 1.6],'FontSize',10,'FontName',fontname);
        uicontrol('Style','text','String',')','Units','characters',...
            'Position',[61.8 16.77  2.2 1.6],'FontSize',10,'FontName',fontname);

        uicontrol('Style','text','String','–азрешение:','Units','characters',...
            'Position',[33.8 14.1 16.2 1.6],'FontName',fontname);
    catch
        disp(lasterr);
    end
    
    try
        % —оздаем обязательные элементы (едиты)
        heditOmega1 = uicontrol('Style','edit','Units','characters',...
            'Position',[35.8 20.77 12.2 1.6],'HorizontalAlignment','right', ...
            'BackgroundColor',[1 1 1],'FontName',fontname,'String',omega1,...
            'Callback','dielectric_gui(''editLoHiFreq_Callback'',gcbo,''omega1'')');    
        heditOmega2 = uicontrol('Style','edit','Units','characters',...
            'Position',[49.8 20.77 12.2 1.6],'HorizontalAlignment','right', ...
            'BackgroundColor',[1 1 1],'FontName',fontname,'String',omega2,...
            'Callback','dielectric_gui(''editLoHiFreq_Callback'',gcbo,''omega2'')');
        
        heditNu1 = uicontrol('Style','edit','Units','characters',...
            'Position',[35.8 18.77 12.2 1.6],'HorizontalAlignment','right', ...
            'BackgroundColor',[1 1 1],'FontName',fontname,'String',nu1,...
            'Callback','dielectric_gui(''editLoHiFreq_Callback'',gcbo,''nu1'')');    
        heditNu2 = uicontrol('Style','edit','Units','characters',...
            'Position',[49.8 18.77 12.2 1.6],'HorizontalAlignment','right', ...
            'BackgroundColor',[1 1 1],'FontName',fontname,'String',nu2,...
            'Callback','dielectric_gui(''editLoHiFreq_Callback'',gcbo,''nu2'')');
        
        heditLamda1 = uicontrol('Style','edit','Units','characters',...
            'Position',[35.8 16.77 12.2 1.6],'HorizontalAlignment','right', ...
            'BackgroundColor',[1 1 1],'FontName',fontname,'String',lamda1,...
            'Callback','dielectric_gui(''editLoHiFreq_Callback'',gcbo,''lamda1'')');    
        heditLamda2 = uicontrol('Style','edit','Units','characters',...
            'Position',[49.8 16.77 12.2 1.6],'HorizontalAlignment','right', ...
            'BackgroundColor',[1 1 1],'FontName',fontname,'String',lamda2,...
            'Callback','dielectric_gui(''editLoHiFreq_Callback'',gcbo,''lamda2'')');
        
        heditResolution = uicontrol('Style','edit','Units','characters',...
            'Position',[51.8 14.46 10.2 1.6],'HorizontalAlignment','right', ...
            'BackgroundColor',[1 1 1],'FontName',fontname,'String',resolution);
    catch
        errordlg(lasterr);
    end

elseif ischar(varargin{1})
    
	try
		feval(varargin{:});
	catch
		errordlg(lasterr);
	end
    
end

% --------------------------------------------------------------------
% ‘ункция вызывается при нажатии кнопки с именем хим. элемента
% для соответствующего элемента создает графическое изображение
% диэлектрической проницаемости
function pushElement(h)
global alkaline halohen element;
% Ќазвание хим. элемента получаем из строки на кнопке
element = get(h,'String');
% ќпределяем галоген и щелечной металл из строки типа Na,Cl хранимой в параметре UserData на кнопке
usdata = get(h,'UserData');
for i=1:length(usdata)
    if usdata(i) == ','
        alkaline=usdata(1:i-1);
        halogen=usdata(i+1:length(usdata));
    end
end
% «агрузка физических параметров веществ
phis;
% —читываем параметры спектра (омега1 омега2 разрешение)
read_spectrum_params_nothrow;
% ќтображаем параметры хим. элемента в рамке 'параметры'
display_parameters_nothrow;
% ¬ызываем функцию графического отображения параметров хим. элемента
% параметры передаются через глобальные переменные
plot_gui;

% --------------------------------------------------------------------
% ‘ункция действует аналогично pushElement
function varargout = pushbuttonOK()
read_parameters_nothrow;
read_spectrum_params_nothrow;
display_parameters_nothrow;
plot_gui;

% --------------------------------------------------------------------
% —читывает параметры хим. соединения из соответствующей рамки
function read_parameters_nothrow()
global m1amu m2amu density epsilonlight epsiloninfrared;
global heditM1amu heditM2amu heditDensity heditEpsilonLight heditEpsilonIR;

try
    m1amu=abs(str2num(get(heditM1amu,'String')));
catch
    disp(lasterr);
end

try
    m2amu=abs(str2num(get(heditM2amu,'String')));
catch
    disp(lasterr);
end

try
    density=abs(str2num(get(heditDensity,'String')));
catch
    disp(lasterr);
end

try
    epsilonlight=abs(str2num(get(heditEpsilonLight, 'String')));
catch
    disp(lasterr);
end

try
    epsiloninfrared=abs(str2num(get(heditEpsilonIR, 'String')));
catch
    disp(lasterr);
end

% --------------------------------------------------------------------
% ¬ыводит параметры хим. соединения в рамку 'ѕараметры соед-я'
function display_parameters_nothrow()
global m1amu m2amu density epsilonlight epsiloninfrared;
global heditM1amu heditM2amu heditDensity heditEpsilonLight heditEpsilonIR;

try
    set(heditM1amu,'String',m1amu);
catch
    disp(lasterr);
end

try
    set(heditM2amu,'String',m2amu);
catch
    disp(lasterr);
end

try
    set(heditDensity,'String',density);
catch
    disp(lasterr);
end

try
    set(heditEpsilonLight,'String',epsilonlight);
catch
    disp(lasterr);
end

try
    set(heditEpsilonIR,'String',epsiloninfrared);
catch
    disp(lasterr);
end

% --------------------------------------------------------------------
% —читывает параметры спектра из рамки '—пектр'
function read_spectrum_params_nothrow()
global heditOmega1 heditOmega2 heditResolution;
global omega1 omega2 resolution;

try
    omega1 = abs(str2num(get(heditOmega1,'String')));
catch
    errordlg(lasterr);
end

try
    omega2 = abs(str2num(get(heditOmega2,'String')));
catch
    errordlg(lasterr);
end

try
    resolution=str2num(get(heditResolution,'String'));
catch
    errordlg(lasterr);
end

% --------------------------------------------------------------------
% strcmp не проверена
function editLoHiFreq_Callback(h, param)
global heditOmega1 heditOmega2;
global heditNu1 heditNu2;
global heditLamda1 heditLamda2;

newvalstr = get(h,'String');
eval(['global ' param ';' param '=' newvalstr ';']);

newval = str2num(newvalstr);
purepar = param(1:length(param)-1);
indx=param(length(param));
if strcmp(purepar,'omega')
    h1 = eval(['global heditNu' indx]);
    h2 = eval(['global heditLamda' indx]);
    val1 = omega2wave(newval);
    val2 = omega2lamda(newval);
elseif strcmp(purepar,'nu')
    h1 = eval(['global heditOmega' indx]);
    h2 = eval(['global heditLamda' indx]);
    val1 = wave2omega(newval);
    val2 = omega2lamda(val1);
elseif strcmp(purepar,'lamda')
    h1 = eval(['heditOmega' indx]);
    h2 = eval(['heditNu' indx]);
    val1 = lamda2omega(newval);
    val2 = omega2wave(val1);
end
set(h1,'String',val1);
set(h2,'String',val2);
